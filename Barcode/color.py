from cv2 import cv2
import numpy as np

color = input("input the color")
font = cv2.FONT_HERSHEY_SIMPLEX
obj_area = 800
area = 0
count = 0
detect = 0

if color == "RED":
    lower = np.array([0, 127, 128])
    upper = np.array([10, 255, 255])
elif color == "GREEN":
    lower = np.array([35, 110, 106])
    upper = np.array([77, 255, 255])  
else:
    lower = np.array([78,43,46])
    upper = np.array([110,255,255])

cap = cv2.VideoCapture(0)
if (cap.isOpened()):  # 视频打开成功
    flag = 1
else:
    flag = 0


if(flag):
    while(1):
        ret, frame = cap.read() #读取一帧
        if ret == False: #检测是否读取成功
            break
        imgHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        imgThresholded = cv2.inRange(imgHSV,lower,upper)
        imgThresholded = cv2.medianBlur(imgThresholded,7)
        contours, hierarchy = cv2.findContours(imgThresholded, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

        for cnt in contours:
            (x, y, w, h) = cv2.boundingRect(cnt)            # moment = cv2.moments(cnt,False)
            #1.面积
            area = cv2.contourArea(cnt)
            print(area)
            if area >= obj_area:
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
                cv2.putText(frame, color, (x, y - 5), font, 0.7, (0, 255, 0), 2)
                count += 1
            
        cv2.imshow("result", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

if count >= 1:
    detect = 1
print(detect)
print(count)
cap.release()
cv2.destroyAllWindows()