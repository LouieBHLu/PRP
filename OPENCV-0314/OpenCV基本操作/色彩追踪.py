import cv2
import numpy as np

picture=cv2.imread('C:/Users/86181/Desktop/opencv/flower.jpg')
cv2.imshow('flowers',picture)
new=cv2.cvtColor(picture,cv2.COLOR_BGR2HSV)
lower=np.array([26,43,46])
upper=np.array([43,255,255])
mask=cv2.inRange(new,lower,upper)

cv2.imshow('mask',mask)

new  = cv2.bitwise_or(picture,mask)

cv2.imwrite('C:/Users/86181/Desktop/opencv/new.jpg',new)
cv2.waitKey(0)
cv2.destroyAllWindows()



