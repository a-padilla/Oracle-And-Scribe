# import libraries
try:
    from PIL import Image
except ImportError:
    import Image
import pytesseract
import numpy as np

# get text from an image as a string
text_from_image = pytesseract.image_to_string(Image.open('images/image_to_translate.jpg'))

# separate string into lines
def strippy(s):
    return s.strip()

lines = text_from_image.split('\n')
clean = np.array(lines)
stripper = np.vectorize(strippy)
clean = stripper(clean)
clean = clean[clean!='']

for i in range(len(clean)):
    print(clean[i])
