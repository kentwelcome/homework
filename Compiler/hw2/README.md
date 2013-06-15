編譯環境：

	windows XP SP3

	1.Dev c++

	2.Borland c++ Builder 2007

使用說明：

	scanner:

	首先，用scanner.c，執行scanner

	1.直接讀取檔案，參數接要開啟的檔案名稱
		E.x >./scanner test_program.c
	2.直接從stdin輸入>./scanner < test_program.c

	輸出結果會直接列印在螢幕上，同時會把所有結果輸出至檔案out.txt中

	parser:

	grammar file的格式是

	a.	lefthandside → righthandside1 | righthandside2 | λ

	每行都必須有換行符號，在9562204.c中，λ因為是2byte的組合字，所以用f代表顯示。
	
	執行過程中會產生兩個檔案 predict.csv 和 table.csv

	這是parse table，可以用excel直接開啟


	1.

	編譯 9562204.c 後，執行請接參數 要讀取的grammar file 和 要從scanner 讀來的file

		e.x >  ./9562204 grammar.txt out.txt
	若沒有下這兩項參數，則預設是開grammar.txt和out.txt這兩個file name

	輸出會在螢幕上以prefix來顯示 parse tree

	若有錯誤則會顯示錯誤的rule

	2.
	
	執行project1.exe，接著在視窗左上角file表單裡選擇要讀取的grammar和scan file

	選擇完畢後，按下畫面底端的parsing，結果會以樹狀圖顯示在畫面中間。

	若有錯誤則會跳出錯誤視窗，並在視窗底下顯示錯誤的rule

	

附檔列表：

	parser.c 		:parser
	scaner.c	:scanner
	Project1.exe		:parser with GUI

	test1.c test2.c		:test file
	grammar.c		:要讀取的grammar
	out.txt			:scanner 的產物
	bcb			:parser with GUI 的 source code 為Borland c++ Builder 2007的project
	
