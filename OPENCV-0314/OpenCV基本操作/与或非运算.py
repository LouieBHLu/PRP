import cv2
import numpy as np

pic1=cv2.imread('C:/Users/86181/Pictures/Saved Pictures/Linux.jpg')
pic2=cv2.imread('C:/Users/86181/Pictures/Saved Pictures/Windows.jpg')
pic_and = cv2.bitwise_and(pic1,pic2)
pic_or = cv2.bitwise_or(pic1,pic2)
pic_not=cv2.bitwise_not(pic1)

cv2.imwrite('C:/Users/86181/Desktop/opencv/and.jpg',pic_and )
cv2.imwrite('C:/Users/86181/Desktop/opencv/or.jpg',pic_or )
cv2.imwrite('C:/Users/86181/Desktop/opencv/not.jpg',pic_not)

cv2.waitKey(0)
cv2.destroyAllWindows()
