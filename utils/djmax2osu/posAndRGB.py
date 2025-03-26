import cv2

def show_frame_and_pixel_info(video_path):
    # Open the video file
    cap = cv2.VideoCapture(video_path)

    # Check if the video was opened successfully
    if not cap.isOpened():
        print(f"Error: Could not open video {video_path}")
        return

    # Get total number of frames
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    # Set the initial frame position
    frame_number = 0
    frame = None  # Initialize frame variable

    # Function to display RGB values on mouse click
    def show_pixel_values(event, x, y, flags, param):
        nonlocal frame  # Access the frame variable from the outer scope
        if event == cv2.EVENT_LBUTTONDOWN and frame is not None:
            b, g, r = frame[y, x]
            print(f"Clicked at ({x}, {y}) - RGB: ({r}, {g}, {b})")

    # Create a window and set the mouse callback function
    cv2.namedWindow('Frame')
    cv2.setMouseCallback('Frame', show_pixel_values)

    while True:
        # Set the frame position
        cap.set(cv2.CAP_PROP_POS_FRAMES, frame_number)

        # Read the frame
        ret, frame = cap.read()
        if not ret:
            print(f"Error: Could not read frame {frame_number} from video {video_path}")
            break

        # Display frame number
        print(f"Current frame: {frame_number} / {total_frames - 1}")

        # Display the frame
        cv2.imshow('Frame', frame)

        # Capture key press
        key = cv2.waitKey(0) & 0xFF
        
        if key == ord('q'):
            break
        elif key == ord('a'):
            frame_number = max(0, frame_number - 1)
        elif key == ord('d'):
            frame_number = min(total_frames - 1, frame_number + 1)
        elif key == ord('z'):
            frame_number = max(0, frame_number - 10)
        elif key == ord('c'):
            frame_number = min(total_frames - 1, frame_number + 10)

    # Release the video capture object and close all OpenCV windows
    cap.release()
    cv2.destroyAllWindows()

# Get video path from user
video_path = "video.mp4"

# Show the frame and pixel info
show_frame_and_pixel_info(video_path)