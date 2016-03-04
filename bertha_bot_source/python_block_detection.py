from PIL import Image

w = 640
h = 480

image = Image.open('/home/pi/0.jpg')

pixels = image.load()

for i in range(0,w):
    for j in range(0,h):
        white = True
        for each in pixels[i,j]:
            if not (each < 80 or each > 155):
                white = False
        if(white):
            pixels[i,j] = (255,255,255)
        else:
            pixels[i,j] = (0,0,0)

image.save('/home/pi/0_filtered.jpg')            

#for i in range(0,w):
   # for j in range(0,h):
       # if ( i < 120 or j < 120 or i > w - 120 or j > h - 80):
            #pixels[i,j] = (0,0,0)

for i in range(120,w-120):
    count = 0
    for j in range(120,h-80):
        if (pixels[i,j] == (255,255,255)):
            count += 1
    if (count > 90):
        for j in range(0,h):
            pixels[i,j] = (255,0,0)
        i = w


image.save('/home/pi/0_targeted.jpg')
