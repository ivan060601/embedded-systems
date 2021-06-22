import numpy as np
import cv2
from numpy.lib.financial import rate

def gstreamer_pipeline(
    capture_width=1280,
    capture_height=720,
    display_width=1280,
    display_height=720,
    framerate=30,
    flip_method=0,
):
    return (
        "nvarguscamerasrc ! "
        "video/x-raw(memory:NVMM), "
        "width=(int)%d, height=(int)%d, "
        "format=(string)NV12, framerate=(fraction)%d/1 ! "
        "nvvidconv flip-method=%d ! "
        "video/x-raw, width=(int)%d, height=(int)%d, format=(string)BGRx ! "
        "videoconvert ! "
        "video/x-raw, format=(string)BGR ! appsink max-buffers=1 drop=true"
        % (
            capture_width,
            capture_height,
            framerate,
            flip_method,
            display_width,
            display_height,
        )
    )

def get_H(color):
    color_new = np.uint8([[color]])
    return int(cv2.cvtColor(color_new, cv2.COLOR_BGR2HSV)[:,:,0])

def create_rects(frame, height, width, colors, thicknesses):
    n = len(colors)
    rect_size = int(width/(n + 2))
    all_rects = []
    margin = int((width - rect_size*n) /(n+1))
    coords = []

    for i in range(0, n):
        start_point = (int(rect_size/2) , int(margin*(i+1) + rect_size * i))
        end_point = (int(rect_size*3/2), int((i+1)*(margin + rect_size)))
        coords.append((start_point, end_point))
        all_rects.append(cv2.rectangle(frame, start_point, end_point, colors[i], thicknesses[i]))

    return all_rects, coords

def renew_colors(colors):
    n = len(colors[0])
    for i in range(0, n):
        colors[0][i] = (list(np.random.random(size=3) * 256))
        print(str(i + 1) +  'st colour = ' + str(get_H(colors[0][i])))
        colors[1][i] = 2

def process(frame, colors):
    h_sensivity = 20
    s_h = 255
    v_h = 255
    s_l = 50
    v_l = 50
    width, height, channels = frame.shape
    
    rects, coords = create_rects(frame, height, width, colors[0], colors[1])
    
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.7

    for i in range(0, len(rects)):
        hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        needed_color_H = get_H(colors[0][i])
        upper = np.array([needed_color_H + h_sensivity, s_h, v_h])
        lower = np.array([needed_color_H - h_sensivity, s_l, v_l])
        start_point = coords[i][0]
        end_point = coords[i][1]

        mask_frame = hsv_frame[start_point[1]:end_point[1] + 1, start_point[0]:end_point[0] + 1]
        mask_needed_color = cv2.inRange(mask_frame, lower, upper)

        rate = np.count_nonzero(mask_needed_color)/((end_point[0]-start_point[0])**2)

        av_hue = np.average(mask_frame[:,:,0])
        av_sat = np.average(mask_frame[:,:,1])
        av_val = np.average(mask_frame[:,:,2])
        average = [int(av_hue),int(av_sat),int(av_val)]

        print(str(average) + " " + str(rate))
    
        #new_text = cv2.putText(start_point, str(average) + " " + str(rate), end_point, font, fontScale, colors[0][i], 2, cv2.LINE_AA)

        if (colors[1][i] != -1):
            if rate > 0.7:
                colors[1][i] = -1
            else:
                text = cv2.putText(rects[i], ' rejected ', end_point, font, fontScale, colors[0][i], 2, cv2.LINE_AA)
        else:
            text = cv2.putText(rects[i], ' passed ', end_point, font, fontScale, colors[0][i], 2, cv2.LINE_AA)

        if (max(colors[1]) == -1):
            renew_colors(colors)
            print('Password changed')

    return frame

print('Press 4 to Quit the Application\n')

# array of requested colours and thicknesses
colors = [[
            (255, 0, 0),
            (0, 255, 0),
            (0, 0, 255)
            #,
            #list(np.random.random(size=3) * 256),
            #list(np.random.random(size=3) * 256),
            #list(np.random.random(size=3) * 256),
            #list(np.random.random(size=3) * 256)
        ], [2, 2, 2]]

for i in range(0, len(colors[0])):
    print(get_H(colors[0][i]))

#Open Default Camera
cap = cv2.VideoCapture(0)#gstreamer_pipeline(flip_method=4), cv2.CAP_GSTREAMER)

while(cap.isOpened()):
    #Take each Frame
    ret, frame = cap.read()
    
    #Flip Video vertically (180 Degrees)
    frame = cv2.flip(frame, 180)

    invert = process(frame, colors)

    # Show video
    cv2.imshow('Cam', frame)

    # Exit if "4" is pressed
    k = cv2.waitKey(1) & 0xFF
    if k == 52 : #ord 4
        #Quit
        print ('Good Bye!')
        break
    if k == 53 :
        renew_colors(colors)

#Release the Cap and Video   
cap.release()
cv2.destroyAllWindows()
