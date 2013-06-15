執行方法：
	1.直接讀取檔案，參數接要開啟的檔案名稱
		E.x kent@kentBSD[~/compiler]>./scanner test_program.c
	2.直接從stdin輸入
		E.x kent@kentBSD[~/compiler]>cat test_program.c | ./scanner

	輸出結果會直接列印在螢幕上，同時會把所有結果輸出至檔案out.txt中


這次作業大概的作法是，首先先開啟要讀取的檔案，或者是直接從stdin抓東西來讀。

檔案開啟以後，用fgets一次讀取一整行存進暫存區中，然後再從第一個byte開始判斷

把kerword、ID、NUM、symbol用is else if來判斷是否滿足條件，match的話就顯示到螢幕上，

同時輸出到檔案out.txt中。

顯示時，會先顯示行數還有原始的程式碼，接著才會列印出match到的token，如果是不合法的token，

則會顯示unknow。
