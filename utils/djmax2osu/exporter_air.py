import cv2
import json
import numpy as np
import os
import base64
import concurrent.futures
import hashlib

from openai import OpenAI
client = OpenAI()

from datetime import datetime

BASE_PATH = "files/" + input("Enter the folder name: ")
channel = input("Enter the channel name: ")

lines = [
    (50, 630),   # Line 1
    (160, 630),  # Line 2
    (277, 630),  # Line 3
    (403, 630)   # Line 4
]

# 배경색
bg_color = np.array([77, 77, 77])
# 마디선 색
l_color = np.array([101, 101, 101])

# mp4 file names
total_base = []
TOFFSET = -3050 + 0

# (1361, 253) to (1803, 364)
OCR_CROP = (1361, 253, 1803, 464)

def get_timestamp():
    """Return current UTC time in YYYY-MM-DD HH:MM:SS format"""
    return datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')

def get_timestamp_name():
    """Return current UTC time in YYYY-MM-DD HH:MM:SS format"""
    return datetime.utcnow().strftime('%Y-%m-%d %H-%M-%S')

failed = []

def parse_notes_from_video(index):
    global failed
    basename = total_base[index - 1]
    video_path = f"{BASE_PATH}/video/{basename}.mp4"

    # get hash of video
    hash = hashlib.sha256()
    with open(video_path, "rb") as file:
        while True:
            data = file.read(65536)
            if not data:
                break
            hash.update(data)
    video_hash = hash.hexdigest()
    print(f"[{get_timestamp()}] Video hash: {video_hash}")
    
    def parse_video():
        cache_path = f"cache/notes_{video_hash}.json"
        if os.path.exists(cache_path):
            with open(cache_path, "r") as file:
                cache = json.load(file)
                return cache
        # Initialize states for each line
        states = ["empty", "empty", "empty", "empty"]

        # Initialize frame numbers for note starts
        note_start_frames = [0, 0, 0, 0]

        # Results list
        results = []

        # Open the video file
        cap = cv2.VideoCapture(video_path)

        if not cap.isOpened():
            print(f"[{get_timestamp()}] Error: Could not open video file {video_path}")
            return

        # Get video properties
        fps = cap.get(cv2.CAP_PROP_FPS)
        if fps <= 0:
            print(f"[{get_timestamp()}] Warning: Could not determine FPS. Using default of 30.")
            fps = 30

        # Function to convert frame number to milliseconds
        append_frame = fps * 1
        def frame_to_ms(frame_num):
            return int((frame_num + append_frame) * (1000 / fps))

        # Process each frame
        frame_num = fps * 2
        total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        print(f"[{get_timestamp()}] Processing video with {total_frames} frames at {fps} FPS...")

        while True:
            ret, frame = cap.read()
            if not ret:
                break
            
            # Progress update
            if frame_num % 600 == 0:
                progress = (frame_num / total_frames) * 100 if total_frames > 0 else 0
                print(f"[{get_timestamp()}] Processing frame {frame_num}/{total_frames} ({progress:.1f}%)")

            # Check each line for notes
            for i, (x, y) in enumerate(lines):
                if y >= frame.shape[0] or x >= frame.shape[1]:
                    continue  # Skip if coordinates are out of bounds
                    
                # Get the color at the current position
                color = frame[y, x]
                
                # Check if the color is not (77, 77, 77) - indicating a note exists
                # different from the background color
                diff_color = np.abs(color - bg_color)
                # check diff color is less than 10
                lt10 = np.all(diff_color < 22)
                diff_color = np.abs(color - l_color)
                lt10x = np.all(diff_color < 22)
                is_note = not lt10 and not lt10x
                
                if is_note:
                    # Note exists
                    if states[i] == "long note pressing":
                        # Continue following the long note
                        pass
                    elif states[i] == "long note maybe":
                        # Change state to long note pressing
                        states[i] = "long note pressing"
                    elif states[i] == "note first frame":
                        # Change state to long note pressing
                        states[i] = "long note maybe"
                    elif states[i] == "empty":
                        # Change state to note first frame and save start frame
                        states[i] = "note first frame"
                        note_start_frames[i] = frame_num
                else:
                    # No note exists
                    if states[i] != "empty":
                        if states[i] == "note first frame":
                            # Add a single note
                            results.append({
                                "type": "single",
                                "line": i + 1,
                                "time": frame_to_ms(note_start_frames[i])
                            })
                        elif states[i] == "long note maybe":
                            # Add a single note
                            results.append({
                                "type": "single",
                                "line": i + 1,
                                "time": frame_to_ms(note_start_frames[i])
                            })
                        elif states[i] == "long note pressing":
                            # Add a long note
                            results.append({
                                "type": "long",
                                "line": i + 1,
                                "time": frame_to_ms(note_start_frames[i]),
                                "etime": frame_to_ms(frame_num)
                            })
                        # Reset state to empty
                        states[i] = "empty"
            
            frame_num += 1

        # Handle any active notes at the end of the video
        for i in range(len(lines)):
            if states[i] == "note first frame":
                results.append({
                    "type": "single",
                    "line": i + 1,
                    "time": frame_to_ms(note_start_frames[i])
                })
            elif states[i] == "long note pressing":
                results.append({
                    "type": "long",
                    "line": i + 1,
                    "time": frame_to_ms(note_start_frames[i]),
                    "etime": frame_to_ms(frame_num)
                })

        # Close the video file
        cap.release()

        # Sort results by time
        results.sort(key=lambda x: x["time"])

        # Save results to cache
        with open(cache_path, "w") as file:
            json.dump(results, file)

        return results
    
    results = parse_video()
    print(f"[{get_timestamp()}] Parsing complete. Found {len(results)} notes.")
    
    def tryOcr():
        # find cache in cache folder
        cache_path = f"cache/ocr_{video_hash}.json"
        if os.path.exists(cache_path):
            with open(cache_path, "r") as file:
                cache = json.load(file)
                return cache
        # try up to 5 times

        def tryOcr__():
            image_path = f"{BASE_PATH}/image/{basename}.png"
            
            # Read the image and convert to base64

            with open(image_path, "rb") as file:
                image_data = file.read()
                # Convert binary data to image
                image = cv2.imdecode(np.frombuffer(image_data, np.uint8), cv2.IMREAD_COLOR)
                cropped_image = image[OCR_CROP[1]:OCR_CROP[3], OCR_CROP[0]:OCR_CROP[2]]
                # Encode the cropped image back to binary
                _, image_data = cv2.imencode('.png', cropped_image)
                image_data = image_data.tobytes()
                image_base64 = base64.b64encode(image_data).decode("utf-8")

            response = client.responses.create(
                model="gpt-4o",
                input=[
                    {
                    "role": "system",
                    "content": [
                        {
                        "type": "input_text",
                        "text": "You are an music information OCR machine. You should give the user the name of the song and artist as a JSON object.\n**A text that is located between name and BPM is artist**. So the artist can not be on top.\nThe name can be two rows. Also the name can have subtitle, just append the name.\nAfter all you should give the user no unicode charactered name as nu_name and no unicode characted name as nu_artist. In here the no unicode character means it should be able to use as a filename. So you MUST not use characters like `/` or `:`. Replace them into space.\n\n# Example\n\n## Case 1\n\n### Input\n[File]\n\n### Output\n{\"name\":\"glory MAX -나의 최대치로 너와 함께할게-\",\"nu_name\":\"glory MAX -To be maximum with you-\",\"artist\":\"TAK\",\"nu_artist\":\"TAK\"}\n\n## Case 2\n\n### Input\n[File]\n\n### Output\n{\"name\":\"애국가\",\"nu_name\":\"Aegukga\",\"artist\":\"안익태\",\"nu_artist\":\"Ahn Eak-tai\"}"
                        }
                    ]
                    },
                    {
                    "role": "user",
                    "content": [
                        {
                        "type": "input_image",
                        "image_url": "data:image/png;base64," + image_base64
                        }
                    ]
                    }
                ],
                text={
                    "format": {
                    "type": "text"
                    }
                },
                reasoning={},
                tools=[],
                temperature=1,
                max_output_tokens=2048,
                top_p=1,
                store=True
            )
            print(f"[{get_timestamp()}] OCR response({video_hash}, {basename}): {response.output_text}")
            # parse response
            jsondt = json.loads(response.output_text.replace("```json", "").replace("```", ""))

            title = jsondt["name"]
            artist = jsondt["artist"]
            nu_name = jsondt["nu_name"]
            nu_artist = jsondt["nu_artist"]
            return jsondt

        for i in range(5):
            try:
                print(f"[{get_timestamp()}] Trying OCR attempt {i + 1}...")
                ocrData = tryOcr__()
                # save to cache
                with open(cache_path, "w") as file:
                    json.dump(ocrData, file)
                return ocrData
            except Exception as e:
                print(f"Error: {e}")
        return None
    
    ocr_result = tryOcr()
    if ocr_result is None:
        print(f"[{get_timestamp()}] Error: OCR failed for video {index}")
        failed.append((index, video_hash))
        return
    title = ocr_result["name"]
    artist = ocr_result["artist"]
    nu_name = ocr_result["nu_name"]
    nu_artist = ocr_result["nu_artist"]

    print(f"[{get_timestamp()}] OCR complete. Title: {title}, Artist: {artist}")

    hit_objects = []
    for n in results:
        line_x = [-1, 0, 128, 256, 384]
        if n["type"] == "single":
            hit_objects.append(f"{line_x[n['line']]},192,{n['time'] + TOFFSET},1,0,1:0:0:100:")
        else:
            if n['time'] == 3000:
                continue
            hit_objects.append(f"{line_x[n['line']]},192,{n['time'] + TOFFSET},128,0,{n['etime'] + TOFFSET}:1:0:0:100:")

    timeStr = get_timestamp_name()

    BASE = f"""osu file format v128

[General]
AudioFilename: audio{basename}-{timeStr}.mp3
AudioLeadIn: 0
PreviewTime: -1
Countdown: 0
SampleSet: Normal
StackLeniency: 0.7
Mode: 3
LetterboxInBreaks: 0
SpecialStyle: 0
WidescreenStoryboard: 1

[Editor]
DistanceSpacing: 1
BeatDivisor: 8
GridSize: 0
TimelineZoom: 1

[Metadata]
Title: {title}
TitleUnicode: {nu_name}
Artist: {artist}
ArtistUnicode: {nu_artist}
Creator: Oein
Version: Air Channel {channel} {timeStr}
Source: DJMAX
Tags: DJMAX {video_hash}__{basename}

[Difficulty]
HPDrainRate: 5
CircleSize: 4
OverallDifficulty: 4
ApproachRate: 4
SliderMultiplier: 1.3999999687075613
SliderTickRate: 1

[Events]

[TimingPoints]
0,1000,4,1,0,100,1,0


[HitObjects]
"""
    output = BASE + "\n".join(hit_objects)

    # create folder with name f"{index} {title}"
    os.makedirs(f"exports/{nu_name}", exist_ok=True)

    # Write the output to a file
    output_path = f"exports/{nu_name}/{basename} {nu_name} - {timeStr}.osu"
    with open(output_path, "w", encoding="utf-8") as file:
        file.write(output)

    print(f"[{get_timestamp()}] Export complete. Saved to {output_path}")

    def extract_audio():
        # check audio cache exists
        audio_cache_path = f"cache/audio_{video_hash}.mp3"
        if os.path.exists(audio_cache_path):
            # copy audio from cache
            os.system(f"cp {audio_cache_path} \"exports/{nu_name}/audio{basename}-{timeStr}.mp3\"")
            return
        # run ffmpeg to extract audio
        os.system(f"ffmpeg -y -i \"{video_path}\" \"cache/audio_{video_hash}.mp3\"")
        os.system(f"cp \"cache/audio_{video_hash}.mp3\" \"exports/{nu_name}/audio{basename}-{timeStr}.mp3\"")
        print(f"[{get_timestamp()}] Audio extraction complete")

    extract_audio()

def process_video_range(start_index, end_index):
    """Process a range of videos with sequential indexing"""
    for i in range(start_index, end_index + 1):
        try:
            print(f"[{get_timestamp()}] Starting processing of video {i}")
            parse_notes_from_video(i)
            print(f"[{get_timestamp()}] Completed processing of video {i}")
        except Exception as e:
            print(f"[{get_timestamp()}] Error processing video {i}: {str(e)}")

if __name__ == "__main__":
    video_files = len(os.listdir(f"{BASE_PATH}/video"))
    total_base = list(filter(lambda x: x.endswith(".mp4"), os.listdir(f"{BASE_PATH}/video")))
    total_base = list(map(lambda x: x[:-4], total_base))
    print(f"[{get_timestamp()}] Found {video_files} video files in the video folder")
    
    total_videos = len(total_base)
    num_threads = 20
    
    # Calculate videos per thread (distribute evenly)
    videos_per_thread = total_videos // num_threads
    remainder = total_videos % num_threads
    
    # Create thread ranges
    ranges = []
    start_idx = 1
    for i in range(num_threads):
        extra = 1 if i < remainder else 0
        end_idx = start_idx + videos_per_thread + extra - 1
        ranges.append((start_idx, end_idx))
        start_idx = end_idx + 1
    
    print(f"[{get_timestamp()}] Starting multi-threaded processing with {num_threads} threads")
    print(f"[{get_timestamp()}] Thread distribution: {ranges}")
    
    # Create thread pool and submit tasks
    with concurrent.futures.ThreadPoolExecutor(max_workers=num_threads) as executor:
        # Submit each range to the thread pool
        futures = [executor.submit(process_video_range, start, end) for start, end in ranges]
        
        # Wait for all tasks to complete
        concurrent.futures.wait(futures)
    
    print(f"[{get_timestamp()}] All videos processed successfully")
    print(f"[{get_timestamp()}] Failed videos: {failed}")