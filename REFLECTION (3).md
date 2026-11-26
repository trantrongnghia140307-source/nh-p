# REFLECTION: AuctionHub Sorting

## 1. Vì sao mỗi thuật toán hợp với module của nó

**A. Insertion Sort (Live Auction Monitor).** Bid đến gần như đã đúng thứ tự, nên mỗi phần tử mới chỉ phải dịch qua vài vị trí. Số bước là O(n + I) với I là số cặp nghịch thế, nên khi dữ liệu gần sắp xếp thì gần như tuyến tính. Nó cũng sắp xếp tại chỗ và giữ nguyên thứ tự các phần tử bằng nhau.

**B. Selection Sort (Top-K Bid Finder).** Mỗi lượt chọn ra đúng phần tử lớn nhất của đoạn còn lại, nên sau k lượt là có Top-k mà không cần sắp xếp hết. Số lần swap tối đa là n − 1, ít hơn hẳn các thuật toán O(n²) khác. Với tập nhỏ thì O(n²) so sánh vẫn chấp nhận được.

**C. Interchange Sort (Price Anomaly Validator).** Lô chỉ ≤ 50 bản ghi nên n² khoảng 2500 phép so sánh, không đáng kể. Code ngắn, dễ viết đúng và dễ kiểm tra, hợp với công cụ QA cần đáng tin hơn là nhanh. Số lần swap cho biết dữ liệu lộn xộn đến mức nào.

**D. Bubble Sort early stop (Stabilization Detector).** Chỉ cần một lượt duyệt không có swap là biết dữ liệu đã sắp xếp. Nếu dữ liệu đã ổn định thì chỉ mất đúng một lượt, tức O(n). Cờ `swapped` dùng để thoát sớm, còn kết quả STABLE/UNSTABLE chỉ phụ thuộc lượt đầu.

**E. Quick Sort (Full Historical Sorter).** Trung bình O(n log n), sắp xếp tại chỗ, nên hợp với hàng triệu bản ghi. Median-of-three giúp dữ liệu đã sắp xếp hoặc đảo ngược không còn là trường hợp xấu.

## 2. Thời gian chạy Quick Sort trên 100k bản ghi

Biên dịch bằng `g++ -O2 -std=c++17`, dữ liệu gồm 100,000 bid sinh bằng `mt19937` (seed 42).

| Input | Thời gian |
|---|---|
| 100,000 bid ngẫu nhiên | 26 ms |
| 100,000 bid đã sắp xếp | 11 ms |
| 100,000 bid đảo ngược | 27 ms |

Kết quả sau khi sắp xếp được kiểm tra bằng hàm `isSorted`. Ảnh chụp: `screenshot_runtime.png`.

## 3. Một bug đã gặp

**Hiện tượng:** chạy `gen.exe` xong không báo lỗi gì nhưng không có file `large.txt`.

**Nguyên nhân:** `gen.exe` được chạy khi đang đứng trong thư mục `test_data`, trong khi `gen.cpp` ghi vào đường dẫn tương đối `test_data/large.txt`. Chương trình đi tìm `test_data/test_data/` nên mở file ghi thất bại, và vì code không kiểm tra `ofstream` nên nó im lặng bỏ qua.

**Cách phát hiện:** mở thư mục thấy chỉ có hai file `.exe`, không có `large.txt`.

**Cách sửa:** đặt code ở thư mục cha `auctionhub-sorting` và chạy từ đó, còn `test_data` chỉ chứa dữ liệu.

**Bài học:** code đọc/ghi file nên kiểm tra `if (!f)` và in thông báo lỗi.

## 4. Ví dụ prompt dùng đúng quy định (Green Zone)

| # | Prompt | Mức |
|---|---|---|
| 1 | "Giải thích vì sao Insertion Sort chạy nhanh trên dữ liệu gần sắp xếp." | Green |
| 2 | "Median-of-three là gì, và Lomuto khác Hoare ở điểm nào?" | Green |
| 3 | "Đây là vòng lặp Bubble Sort early stop của mình, cờ `swapped` đã đúng chưa?" | Green |

## 5. Worst-case của Quick Sort với median-of-three

Vẫn là O(n²). Median-of-three chỉ làm cho dữ liệu đã sắp xếp hoặc đảo ngược không còn là trường hợp xấu nhất. Vẫn có những dãy được dựng riêng để trung vị luôn lệch về một đầu, khi đó mỗi lần partition chỉ loại được vài phần tử:

T(n) = T(n − 1) + O(n) = O(n²)

Muốn bảo đảm O(n log n) thì cần pivot ngẫu nhiên, introsort hoặc median-of-medians.
