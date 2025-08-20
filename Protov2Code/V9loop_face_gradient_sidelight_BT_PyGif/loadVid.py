import os
import numpy as np
from PIL import Image, ImageSequence
import imageio
import math
import cv2



filepath = "Protov2Code/vids/haruhi.gif"
# filepath = "Protov2Code/vids/badapple.mp4"
outfilepath = "Protov2Code/V9loop_face_gradient_sidelight_BT_PyGif/bitmaps/haruhi.h"
out_fps = 10
output_size = (64, 32)
resamp_mode = "interp" # "interp" or "round"
time_unit = 1



#### IDK if this works or if the img_to_hex file works?
# My guess is teh file.
def convert_frame(frame):
    # Function to convert RGB888 to RGB565
    def rgb888_to_rgb565(r, g, b):
        return ((r.astype(np.uint16) & 0xF8) << 8) | ((g.astype(np.uint16) & 0xFC) << 3) | (b.astype(np.uint16) >> 3)
    
    return rgb888_to_rgb565(frame[:, :, 0], frame[:, :, 1], frame[:, :, 2])




def loadGif(filename):
    # Load in the gif
    original = Image.open(filename)

    # Try to get the frame duration (in milliseconds)
    per_frame_duration_ms = original.info["duration"] # Duration of a frame I think
    total_duration_s = original.n_frames*per_frame_duration_ms / 1000
    fps = 1000 / per_frame_duration_ms
    print(f"GIF FPS: {fps:.2f}")

    # List to hold processed frames
    frames = []

    # Iterate and process each frame
    for frame in ImageSequence.Iterator(original):
        frame = frame.convert("RGB")
        resized = frame.resize(output_size, Image.LANCZOS)
        frame = np.array(resized)
        frames.append(frame)

    # Get the number of frames
    n_frames = len(frames)

    # Proportion of frames we need to create the new fps
    fps_prop = out_fps/fps

    # Subsample the frames
    n_frames_new = math.ceil(fps_prop*n_frames - fps_prop)
    dtype = frames[0].dtype
    new_frames = np.zeros((n_frames_new, output_size[1], output_size[0], 3), dtype=dtype)
    for out_frame_i in range(0, n_frames_new):
        # The frame number
        frame_num = out_frame_i * 1/fps_prop
        # Is this an integer frame?
        if frame_num - math.floor(frame_num) == 0:
            new_frames[out_frame_i] = frames[int(frame_num)]
        else:
            if resamp_mode == "interp":
                # If not, interpolate the floor and ceil
                floor_val = math.floor(frame_num)
                ceil_val = math.ceil(frame_num)
                ceil_weight = frame_num - floor_val
                new_frames[out_frame_i] = np.clip((1-ceil_weight)*frames[floor_val].astype(np.float32) + ceil_weight*frames[ceil_val].astype(np.float32), 0, 255).astype(dtype)
            else:
                # Take the nearest integer multiple
                new_frames[out_frame_i] = frames[round(frame_num)]

    imageio.mimsave("tmp.gif", new_frames, duration=1000/out_fps)

    return np.stack(new_frames), len(new_frames)


def loadMp4(filename):
    cap = cv2.VideoCapture(filename)
    if not cap.isOpened():
        raise IOError(f"Cannot open video file {filename}")

    original_fps = cap.get(cv2.CAP_PROP_FPS)
    frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    original_duration = frame_count / original_fps

    print(f"Original FPS: {original_fps:.2f}")
    print(f"Total frames: {frame_count}")
    print(f"Original duration: {original_duration:.2f} seconds")

    # Determine frame sampling strategy
    if out_fps is None or out_fps >= original_fps:
        frame_step = 1
        actual_fps = original_fps
    else:
        frame_step = math.ceil(original_fps / out_fps)
        actual_fps = original_fps / frame_step

    frames = []
    frame_index = 0

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        if frame_index % frame_step == 0:
            resized = cv2.resize(frame, output_size, interpolation=cv2.INTER_LANCZOS4)
            rgb_frame = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
            frames.append(rgb_frame)

        frame_index += 1

    cap.release()
    new_duration = len(frames) / actual_fps
    print(f"Adjusted FPS: {actual_fps:.2f}")
    print(f"New total frames: {len(frames)}")
    print(f"Adjusted duration: {new_duration:.2f} seconds")

    # imageio.mimsave("tmp.gif", frames, duration=1000/out_fps)

    return frames, len(frames)





def main(filepath):
    filepath = filepath.strip()
    ending = filepath.split(".")[-1]
    if ending == "gif":
        frames, num_frames = loadGif(filepath)
    elif ending == "mp4":
        frames, num_frames = loadMp4(filepath)
    elif filepath == "png" or filepath == "jpg":
        frames, num_frames = loadImg(filepath)
    else:
        assert False
    duration = num_frames / out_fps

    print(f"Output will have {num_frames} frames with fps {out_fps} and duration {duration}")
    

    # Save frames and times to the .h file
    file_str = ""
    # First line is the number of frames
    file_str += f"#define NUM_FRAMES {num_frames}\n\n"

    # Add the frame times
    frame_time = out_fps*time_unit # Number of frames per time_unit seconds
    if math.ceil(frame_time) - frame_time != 0:
        print(f"Frame time is not an integer: {frame_time}")
    frame_time = round(frame_time)
    file_str += "const uint16_t frame_time_unit_ranges[NUM_FRAMES][2] = {\n"
    file_str += f"    {frame_time, frame_time},\n"*num_frames
    file_str += "};\n\n"

    # Add the frames
    vriz = np.vectorize(hex)
    file_str += "uint16_t bitmaps[NUM_FRAMES][HEIGHT][WIDTH2] = {\n"
    for frame_ in frames:
        file_str += "    {\n"
        #frame_ = convert_frame(frame_.transpose((1, 0, 2)))
        frame_ = convert_frame(frame_)
        frame_ = vriz(frame_)
        for row in frame_:
            file_str += ("        {"+ str(row.tolist())[1:-1] + "},\n").replace("'", "")
        file_str += "    },\n"
    file_str += "};\n"

    # Save the file
    with open(outfilepath, "w") as f:
        f.write(file_str)








main(filepath)
