import numpy as np
import pandas as pd
import csv
import matplotlib.pyplot as plt
import mplcursors
from scipy.signal import butter, filtfilt

# Đoạn mã xử lý và vẽ đồ thị PPG và PCG ở trên
with open('Binh_1st.txt', 'r') as file:
    content = file.read()

# Loại bỏ tất cả các dấu cách và thay thế chúng bằng dấu phẩy
content_without_spaces = content.replace("\t", ",")
# Tạo hoặc mở tệp mới để lưu nội dung đã xử lý
with open('sonn.csv', 'w') as new_file:
    # Ghi nội dung đã xử lý vào tệp mới
    new_file.write(content_without_spaces)

# Tên tệp CSV
file_name = 'sonn.csv'

# Đọc dữ liệu PPG từ tệp CSV
ppg_data = []
pcg_data = []
fs = 400
window_size = int(fs * 0.1)

with open(file_name, mode='r') as csv_file:
    csv_reader = csv.reader(csv_file)

    # Duyệt qua từng dòng trong tệp CSV
    for row in csv_reader:
        if len(row) >= 9:
            # Đảm bảo có ít nhất 7 cột trong mỗi dòng
            column_data = float(row[7])
            column_dataa = float(row[6])
            # Lấy dữ liệu từ cột thứ 7 (0-based index)
            ppg_data.append(column_data)
            pcg_data.append(column_dataa)  # Sử dụng dữ liệu từ cột thứ 7 ở đây, ví dụ
            # print(column_data)

# Áp dụng bộ lọc lfilter cho cả hai tín hiệu PPG và PCG
order = 4  # Độ bậc của bộ lọc
cutoff_frequency = 10  # Tần số cắt (đơn vị: Hz)
nyquist_frequency = 0.5 * fs
normal_cutoff = cutoff_frequency / nyquist_frequency
b, a = butter(order, normal_cutoff, btype='low', analog=False)

ppg_data_filtered = filtfilt(b, a, ppg_data)
pcg_data_filtered = filtfilt(b, a, pcg_data)

# Đồ thị PPG và PCG
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(8, 6))

ax1.plot(ppg_data, label='PPG Data')
ax1.plot(ppg_data_filtered, label='Filtered PPG Data', alpha=0.8, lw=3)
ax1.legend()
ax1.set_ylabel('Giá Trị PPG')  # Thêm nhãn cho trục y của đồ thị PPG
ax1.set_title('PPG Data')  # Thêm tiêu đề cho đồ thị PPG

ax2.plot(pcg_data, label='PCG Data')
ax2.plot(pcg_data_filtered, label='Filtered PCG Data', alpha=0.8, lw=3)
ax2.legend()
ax2.set_xlabel('Sample Index')  # Thêm nhãn cho trục x của đồ thị PCG
ax2.set_ylabel('Giá Trị PCG')  # Thêm nhãn cho trục y của đồ thị PCG
ax2.set_title('PCG Data')  # Thêm tiêu đề cho đồ thị PCG

# Sử dụng mplcursors để hiển thị giá trị khi di chuyển chuột qua điểm trên đồ thị
cursor1 = mplcursors.cursor(ax1, hover=True)
cursor2 = mplcursors.cursor(ax2, hover=True)

# Định dạng hiển thị giá trị trên đồ thị
cursor1.connect("add", lambda sel: sel.annotation.set_text(f'PPG: {sel.target[1]:.2f}'))
cursor2.connect("add", lambda sel: sel.annotation.set_text(f'PCG: {sel.target[1]:.2f}'))

plt.show()
