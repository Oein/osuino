import cv2
import numpy as np
import os
import time
from datetime import datetime, timedelta
from moviepy.editor import VideoFileClip, concatenate_videoclips

def get_timestamp():
    """Return current UTC time in YYYY-MM-DD HH:MM:SS format"""
    return datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')

apd = ""

def process_video(input_video_path=f"video{apd}.mp4"):
    """
    Process a video file according to the specified color conditions.
    Preserves audio from the original video.
    """
    # Create output directories if they don't exist
    os.makedirs(f"video{apd}", exist_ok=True)
    os.makedirs(f"image{apd}", exist_ok=True)
    
    print(f"[{get_timestamp()}] Starting video processing")
    
    # Open the video with OpenCV for frame analysis
    cap = cv2.VideoCapture(input_video_path)
    
    if not cap.isOpened():
        print(f"[{get_timestamp()}] Error: Could not open video file {input_video_path}")
        return
    
    # Get video properties
    fps = cap.get(cv2.CAP_PROP_FPS)
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    duration = total_frames / fps
    
    print(f"[{get_timestamp()}] Video loaded: {input_video_path}")
    print(f"[{get_timestamp()}] Video details: {total_frames} frames, {fps:.2f} FPS, {duration:.2f} seconds")
    
    # Open the video with moviepy for handling audio
    full_clip = VideoFileClip(input_video_path)
    
    # State variables
    state = 0  # 0=looking for (23,23,23), 1=looking for (0,0,0), 2=looking for (79,79,79)
    index = 0
    frame_count = 0
    start_frame = 0
    
    # For status reporting
    start_time = time.time()
    last_report_time = start_time
    report_interval = 5  # Report every 5 seconds
    
    while True:
        ret, frame = cap.read()
        
        if not ret:
            print(f"[{get_timestamp()}] End of video reached")
            break
        
        # Status reporting
        current_time = time.time()
        if current_time - last_report_time >= report_interval:
            elapsed_time = current_time - start_time
            progress = frame_count / total_frames if total_frames > 0 else 0
            
            # Calculate processing speed and ETA
            frames_per_second = frame_count / elapsed_time if elapsed_time > 0 else 0
            eta_seconds = (total_frames - frame_count) / frames_per_second if frames_per_second > 0 else 0
            eta = str(timedelta(seconds=int(eta_seconds)))
            
            print(f"[{get_timestamp()}] Processing: {frame_count}/{total_frames} frames "
                  f"({progress:.1%}), Speed: {frames_per_second:.2f} FPS, ETA: {eta}, "
                  f"State: {state}, Current index: {index}")
            
            last_report_time = current_time
        
        # Get the color at position (234, 87)
        # Note: OpenCV uses BGR order and [y, x] format for indexing
        color_bgr = frame[87, 87]
        # Convert from BGR to RGB for comparison
        color_rgb = (int(color_bgr[2]), int(color_bgr[1]), int(color_bgr[0]))
        
        if state == 0:  # Looking for (23, 23, 23)
            if color_rgb == (23, 23, 23):
                print(f"[{get_timestamp()}] Found color (23, 23, 23) - Creating video slice {index}")
                
                # Calculate time range for video slice (up to current frame)
                start_time_video = start_frame / fps
                end_time_video = frame_count / fps
                
                # Extract the clip with audio (up to the current frame where target color was found)
                clip = full_clip.subclip(start_time_video, end_time_video)
                
                # Save the video slice with audio
                output_video_path = os.path.join(f"video{apd}", f"{index}.mp4")
                print(f"[{get_timestamp()}] Saving video to {output_video_path}...")
                clip.write_videofile(output_video_path, codec='libx264', audio_codec='aac', verbose=False, logger=None)
                
                # Skip 20 frames and capture the image after skipping
                image_frame = None
                for _ in range(20):
                    ret, image_frame = cap.read()
                    if not ret:
                        print(f"[{get_timestamp()}] End of video reached while skipping frames")
                        break
                    frame_count += 1
                
                # Save the image after skipping 20 frames
                if image_frame is not None:
                    output_image_path = os.path.join(f"image{apd}", f"{index + 1}.png")
                    print(f"[{get_timestamp()}] Saving frame after 20 frames to {output_image_path}...")
                    cv2.imwrite(output_image_path, image_frame)
                    print(f"[{get_timestamp()}] Saved image for slice {index}")
                
                print(f"[{get_timestamp()}] Saved video slice {index} (Duration: {clip.duration:.2f}s)")
                
                # Move to next state
                state = 1
                index += 1
                start_frame = frame_count + 1
            else:
                frame_count += 1
        
        elif state == 1:  # Looking for (0, 0, 0)
            frame_count += 1
            if color_rgb == (0, 0, 0):
                print(f"[{get_timestamp()}] Found color (0, 0, 0) - Moving to next state")
                state = 2
        
        elif state == 2:  # Looking for (79, 79, 79)
            frame_count += 1
            if color_rgb == (79, 79, 79):
                print(f"[{get_timestamp()}] Found color (79, 79, 79) - Starting over")
                # Go back to state 0 to start capturing frames again
                state = 0
                start_frame = frame_count
    
    # Release resources
    cap.release()
    full_clip.close()
    
    # Final status report
    total_time = time.time() - start_time
    avg_fps = frame_count / total_time if total_time > 0 else 0
    print(f"[{get_timestamp()}] Video processing completed.")
    print(f"[{get_timestamp()}] Processed {frame_count} frames in {total_time:.2f} seconds ({avg_fps:.2f} FPS)")
    print(f"[{get_timestamp()}] Created {index} video slices")

if __name__ == "__main__":
    print(f"[{get_timestamp()}] Video slicer starting...")
    process_video()