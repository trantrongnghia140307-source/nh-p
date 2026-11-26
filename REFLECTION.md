# REFLECTION: AuctionHub Sorting

> Bản mẫu để ôn tập. Mục 3 và 4 là ví dụ minh họa, hãy thay bằng trải nghiệm và prompt thật của bạn nếu nộp.

## 1. Lý do chọn thuật toán cho từng module

**A. Insertion Sort (Live Auction Monitor).** Dữ liệu gần như đã sắp xếp, nên mỗi phần tử mới chỉ phải dịch qua rất ít phần tử lớn hơn nó. Chi phí là O(n + I), với I là số cặp nghịch thế. Khi I ≈ O(n) thì toàn bộ chạy gần tuyến tính, nhanh hơn nhiều so với O(n log n) của các thuật toán tổng quát. Nó cũng sắp xếp tại chỗ (O(1) bộ nhớ phụ) và ổn định.

**B. Selection Sort (Top-K Bid Finder).** Mỗi lượt chọn ra đúng phần tử lớn nhất của đoạn còn lại, nên sau k lượt ta đã có Top-k mà không cần sắp xếp hết. Số lần hoán đổi chỉ tối đa n − 1, hữu ích khi việc ghi/hoán đổi bản ghi tốn kém. Với tập nhỏ, O(n²) so sánh vẫn chấp nhận được.

**C. Interchange Sort (Price Anomaly Validator).** Với lô ≤ 50 bản ghi, n² khoảng 2500 phép so sánh, không đáng kể. Code rất ngắn, ít chỗ sai và dễ kiểm chứng, phù hợp công cụ QA cần độ tin cậy hơn tốc độ. Số lần swap được đếm để đánh giá mức độ lộn xộn của lô dữ liệu.

**D. Bubble Sort early stop (Stabilization Detector).** Chỉ cần một lượt duyệt không có hoán đổi là biết dữ liệu đã sắp xếp. Với dữ liệu đã ổn định, thuật toán dừng sau đúng một lượt, tức O(n). Cờ `swapped` cho phép thoát sớm, còn kết quả "STABLE/UNSTABLE" được quyết định riêng ở lượt đầu tiên.

**E. Quick Sort (Full Historical Sorter).** Trung bình O(n log n), sắp xếp tại chỗ và thân thiện với cache, nên phù hợp hàng triệu bản ghi. Median-of-three tránh trường hợp xấu với dữ liệu đã sắp xếp hoặc đảo ngược, vốn rất hay gặp ở dữ liệu lịch sử.

## 2. Thời gian chạy Quick Sort trên 100k bản ghi

Biên dịch bằng `g++ -O2 -std=c++17`, dữ liệu sinh bằng `mt19937` (seed 42):

| Input | Thời gian |
|---|---|
| 100,000 bid ngẫu nhiên | **~22–23 ms** |
| 100,000 bid đã sắp xếp | ~11 ms |
| 100,000 bid đảo ngược | ~24 ms |

Số đo phụ thuộc máy; hãy chạy lại trên máy bạn và chụp `screenshot_runtime.png`. Kết quả được kiểm tra bằng hàm `isSorted`.

## 3. Một bug (ví dụ minh họa)

**Bug:** Bubble Sort early stop luôn trả về `true`, kể cả khi đầu vào đảo ngược.

**Nguyên nhân:** hàm trả về theo cờ `swapped` của lượt cuối cùng. Lượt cuối của mọi dãy đều không có swap (đó chính là lý do vòng lặp dừng), nên kết quả luôn là "STABLE".

**Cách phát hiện và sửa:** chạy test đảo ngược thấy in `STABLE` thay vì `UNSTABLE`. Tôi tách ra hai biến: `swapped` (dùng để thoát sớm) và `stable` (chỉ được gán `false` nếu `pass == 0 && swapped`).

## 4. Ba prompt đã dùng (ví dụ minh họa)

| # | Prompt | Mức |
|---|---|---|
| 1 | "Giải thích vì sao Insertion Sort hiệu quả với dữ liệu gần sắp xếp." | Green |
| 2 | "Median-of-three hoạt động thế nào, và Lomuto khác Hoare ra sao?" | Green |
| 3 | "Cho mình pseudocode của Bubble Sort có early stop." | Yellow |

Lưu ý: nếu bạn nhờ viết toàn bộ hàm hoặc dán cả đề để lấy lời giải thì đó là Red Zone, và bạn nên ghi đúng như vậy.

## 5. Worst-case của Quick Sort với median-of-three

**Vẫn là O(n²).** Median-of-three chỉ làm cho các input phổ biến (đã sắp xếp, đảo ngược) không còn là trường hợp xấu nhất. Nó không loại bỏ được worst-case: vẫn tồn tại những dãy được dựng riêng sao cho trung vị của ba phần tử luôn là một giá trị gần min hoặc max của đoạn. Khi đó mỗi lần partition chỉ loại được O(1) phần tử, và:

T(n) = T(n − 1) + O(n) = O(n²)

Ngoài ra, partition kiểu Lomuto có thể suy biến khi có rất nhiều phần tử bằng nhau (tie-break theo `timestamp` trong bài này giúp giảm tình trạng đó). Muốn bảo đảm O(n log n) trong mọi trường hợp cần chọn pivot ngẫu nhiên (xác suất cao), introsort, hoặc median-of-medians.
