import sys
import numpy as np
import cv2

# src = cv2.imread('./data/rose.bmp', cv2.IMREAD_GRAYSCALE)
# src = cv2.imread('./data/rose.bmp')
# src = cv2.imread('./data/noise.bmp', cv2.IMREAD_GRAYSCALE)
src = cv2.imread('./data/lenna.bmp')

if src is None:
    print('Image load failed!')
    sys.exit()

cv2.imshow('src', src) # 원본

# for ksize in (3, 5, 7):
#     # 커널 사이즈 별 평균값 필터링
#     dst = cv2.blur(src, (ksize, ksize))

#     desc = 'Mean: {}x{}'.format(ksize, ksize)
#     cv2.putText(dst, desc, (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
#                 1.0, 255, 1, cv2.LINE_AA)
    
#     cv2.imshow('dst' + str(ksize), dst)
#     cv2.waitKey()

# =================================================

# for sigma in range(1, 6):
#     # sigma(가중치) 값을 이용하여 가우시안 필터링
#     dst = cv2.GaussianBlur(src, (0, 0), sigma)

#     desc = 'sigma = {}'.format(sigma)
#     cv2.putText(dst, desc, (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
#                 1.0, 255, 1, cv2.LINE_AA)

#     cv2.imshow('dst' + str(sigma), dst)
#     cv2.waitKey()

# =================================================

# blr = cv2.GaussianBlur(src, (0, 0), 2) // 가중치 2
# dst = np.clip(2.0 * src - blr, 0, 255).astype(np.uint8) # 흑백 샤프닝 필터링 (계수 2.0)

# cv2.imshow('blr', blr)
# cv2.imshow('dst', dst)
# cv2.waitKey()

# =================================================

# src_ycrcb = cv2.cvtColor(src, cv2.COLOR_BGR2YCrCb)

# src_f = src_ycrcb[:, :, 0].astype(np.float32)
# blr = cv2.GaussianBlur(src_f, (0, 0), 2.0) # 밝기 채널만 블러
# src_ycrcb[:, :, 0] = np.clip(2.0 * src_f - blr, 0, 255).astype(np.uint8)

# dst = cv2.cvtColor(src_ycrcb, cv2.COLOR_YCrCb2BGR)

# cv2.imshow('dst', dst)
# cv2.waitKey()

# =================================================

# dst = cv2.medianBlur(src, 3) // 3 x 3 메디안 필터

# cv2.imshow('dst', dst)
# cv2.waitKey()

# =================================================

dst = cv2.bilateralFilter(src, -1, 10, 5) # 10: 색 공간에서 필터의 표준편차, 5: 좌표 공간에서 필터의 표준편차

cv2.imshow('src', src)
cv2.imshow('dst', dst)
cv2.waitKey()

cv2.destroyAllWindows()
