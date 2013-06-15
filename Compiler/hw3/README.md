編譯環境：

	windows XP SP3

	1.Dev c++

	2.Borland c++ Builder 2007

使用說明：

	scanner:
	
	scanner的功能附加在parser裡，不需另外使用。

	parser:

	grammar file的格式是

	a.	lefthandside → righthandside1 | righthandside2 | λ

	每行都必須有換行符號，在lalrParser.c中，λ因為是2byte的組合字，所以用f代表顯示。
	
	執行過程中會產生三個檔案 out.txt , state.txt 和 go_to_table.csv
	
	out.txt為scanner完抓出來的token

	go_to_table.csv可以用excel直接開啟


	1.

	編譯 lalrParser.c 後，執行請接參數 要讀取的 grammar file 和 scanner 要讀的source code

		e.x >  ./parser grammar.txt test.c
	若沒有下這兩項參數，則預設是開grammar.txt和test.c這兩個file name

	輸出會在螢幕上以postoder來顯示 parse tree

	若有錯誤則會顯示錯誤的state

	2.
	
	執行lalr_parser.exe，接著在視窗左上角file表單裡選擇要讀取的grammar和C file

	選擇完畢後，按下畫面底端的parsing，結果會以樹狀圖顯示在畫面中間。

	若有錯誤則會跳出錯誤視窗。

	

附檔列表：

	lalrParser.c 		:parser
	lalr_parser/		:parser with GUI

	test1.c test2.c test3.c test.c :test file
	grammar.c		:要讀取的grammar
	out.txt			:scanner 的產物
	lalr_parser		:parser with GUI 的 source code 為Borland c++ Builder 2007的project
	
