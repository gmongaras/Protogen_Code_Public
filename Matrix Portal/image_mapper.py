from PIL import Image
import numpy as np

# Load the image
image_path = '/home/gmongaras/Documents/MyStuff/Protogen/Programming/Matrix Portal/images/3.png'
image = Image.open(image_path)

# Resize the image to 32x64
image_resized = image.resize((64, 32))

# Convert the image to RGB format
image_rgb = image_resized.convert('RGB')

# Function to convert RGB888 to RGB565
def rgb888_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

# Convert the image to RGB565
rgb565_data = np.zeros((32, 64), dtype=np.uint16)
for y in range(32):
    for x in range(64):
        r, g, b = image_rgb.getpixel((x, y))
        rgb565_data[y, x] = rgb888_to_rgb565(r, g, b)

# Create the formatted output similar to the array provided
formatted_output = []
for row in rgb565_data:
    formatted_row = ', '.join(f'0x{val:04X}' for val in row)
    formatted_output.append(f"{{ {formatted_row} }}")

# Write the formatted output to a file
output_file_path = 'rgb565_image_matrix.txt'
with open(output_file_path, 'w') as f:
    f.write('uint16_t bitmap[32][64] = {\n')
    f.write(',\n'.join(formatted_output))
    f.write('\n};')

# Display the formatted result
output_file_path