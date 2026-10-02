import sys
import numpy as np
import cv2
import math

# src = cv2.imread('./data/tekapo.bmp')
# src = cv2.imread('./data/rose.bmp')
src = cv2.imread('./data/pinkwink_namecard.png')

if src is None:
    print('Image load failed!')
    sys.exit()

# aff = np.array([[1, 0, 200],
#                 [0, 1, 100]], dtype=np.float32) # x로 200, y로 100 이동

# dst = cv2.warpAffine(src, aff, (0, 0)) # 출력 크기 = 입력과 동일

# cv2.imshow('src', src)
# cv2.imshow('dst', dst)

# =================================================

# aff = np.array([[1, 0.5, 0],
#                 [0, 1, 0]], dtype=np.float32) # 가로 방향 전단, m = 0.5

# h, w = src.shape[:2]

# dst = cv2.warpAffine(src, aff, (w + int(h * 0.5), h)) # 밀린 만큼 폭을 늘림

# cv2.imshow('src', src)
# cv2.imshow('dst', dst)

# =================================================

# dst1 = cv2.resize(src, (0, 0), fx=4, fy=4, interpolation=cv2.INTER_NEAREST) # 계단 현상이 뚜렷
# dst2 = cv2.resize(src, (1920, 1280)) # INTER_LINEAR : 부드럽지만 흐릿하다
# dst3 = cv2.resize(src, (1920, 1280), interpolation=cv2.INTER_CUBIC) # linear 보다는 윤곽이 뚜렷
# dst4 = cv2.resize(src, (1920, 1280), interpolation=cv2.INTER_LANCZOS4) # 속도가 느린 대신 품질이 가장 좋다

# cv2.imshow('src', src)
# cv2.imshow('dst1', dst1[500:900, 400:800]) # 같은 영역만 잘라 비교
# cv2.imshow('dst2', dst2[500:900, 400:800])
# cv2.imshow('dst3', dst3[500:900, 400:800])
# cv2.imshow('dst4', dst4[500:900, 400:800])

# =================================================

# dst = cv2.flip(src, -1) // 1: 좌우, 0: 상하, -1:상하좌우

# cv2.imshow('src', src)
# cv2.imshow('dst', dst)

# =================================================

# dst = cv2.pyrDown(src) # resize와 달리 5x5 가우시안 필터를 적용한 후에 행과 열을 제거

# cv2.imshow('src', src)
# cv2.imshow('dst', dst)

# =================================================

# rad = 20 * math.pi / 180 # degree → radian
# aff = np.array([[ math.cos(rad), math.sin(rad), 0],
#                 [-math.sin(rad), math.cos(rad), 0]], dtype=np.float32)

# dst = cv2.warpAffine(src, aff, (0, 0))

# cv2.imshow("src", src)
# cv2.imshow("dst", dst)

# =================================================

# cp = (src.shape[1] / 2, src.shape[0] / 2) # 영상 중앙 (x, y)
# rot = cv2.getRotationMatrix2D(cp, 20, 0.7) # 20도 회전 + 0.7배 축소

# dst = cv2.warpAffine(src, rot, (0, 0))

# cv2.imshow("src", src)
# cv2.imshow("dst", dst)

# =================================================

w, h = 720, 400 # 펴서 만들 결과 영상 크기

srcQuad = np.array([[660, 313], [1696, 438], [1722, 963], [440, 785]], np.float32)
dstQuad = np.array([[0, 0], [w-1, 0], [w-1, h-1], [0, h-1]], np.float32)

pers = cv2.getPerspectiveTransform(srcQuad, dstQuad)
dst = cv2.warpPerspective(src, pers, (w, h))

cv2.imshow('src', src)

cv2.waitKey()

cv2.destroyAllWindows()
