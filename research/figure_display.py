import matplotlib.pyplot as plt
import numpy as np
import cv2

basepath = "/home/shincurry/Dropbox/Paper/animeloop/Resources/"

framediff = []
framediff64 = []
similarities_ahash = []
similarities_dhash = []
similarities_phash = []

with open(basepath+"lens_switch/framediff.txt") as f:
    framediff = map(float, f)
with open(basepath+"lens_switch/framediff64.txt") as f:
    framediff64 = map(float, f)

with open(basepath+"similarities_ahash64.txt") as f:
    similarities_ahash = map(float, f)[0:480]
with open(basepath+"similarities_dhash64.txt") as f:
    similarities_dhash = map(float, f)[0:480]
with open(basepath+"similarities_phash64.txt") as f:
    similarities_phash = map(float, f)[0:480]


# frames = range(1, len(similarities_dhash)+1)
# plt.plot(frames, similarities_dhash, 'b', frames, similarities_phash, 'r', frames, similarities_ahash, 'y')
# plt.show()

frames = range(1, len(framediff)+1)
plt.plot(range(1, len(framediff64)+1), framediff64, 'r')
# plt.plot(range(1, len(framediff)+1), framediff, 'b')
plt.show()