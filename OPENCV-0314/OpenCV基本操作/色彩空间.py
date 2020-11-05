import cv2

picture=cv2.imread('C:/Users/86181/Desktop/opencv/test.jpg')
hsv=cv2.cvtColor(picture,cv2.COLOR_BGR2HSV)                           #转换bgr到hsv
gray=cv2.cvtColor(picture,cv2.COLOR_BGR2GRAY)                         #转换bgr到灰度图
cv2.imshow('test',picture)
cv2.imshow('hsv',hsv)
cv2.imshow('gray',gray)
cv2.imwrite('C:/Users/86181/Desktop/opencv/hsv.jpg',hsv)         #保存到指定路径
cv2.imwrite('C:/Users/86181/Desktop/opencv/gray.jpg',gray)
cv2.waitKey(0)
cv2.destroyAllWindows()