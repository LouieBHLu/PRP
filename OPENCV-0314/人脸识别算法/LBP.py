from skimage.transform import rotate
from skimage.feature import local_binary_pattern
from skimage import data, io,data_dir,filters, feature
from skimage.color import label2rgb
import skimage
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import cv2
radius = 1  # LBP算法中范围半径的取值
n_points = 8 * radius # 领域像素点数
image = cv2.imread('./test_data/test1.png')
#显示到plt中，需要从BGR转化到RGB，若是cv2.imshow(win_name, image)，则不需要转化
image1 = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
plt.imshow(image1)
image = cv2.cvtColor(image1, cv2.COLOR_BGR2GRAY)
plt.imshow(image, plt.cm.gray)
lbp = local_binary_pattern(image, n_points, radius)
plt.imshow(lbp, plt.cm.gray)
edges = filters.sobel(image)
plt.imshow(edges, plt.cm.gray)
plt.show()
cv2.imshow('a',image)
cv2.waitKey(0)