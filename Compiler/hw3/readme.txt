�sĶ���ҡG

	windows XP SP3

	1.Dev c++

	2.Borland c++ Builder 2007

�ϥλ����G

	scanner:
	
	scanner���\����[�bparser�̡A���ݥt�~�ϥΡC

	parser:

	grammar file���榡�O

	a.	lefthandside �� righthandside1 | righthandside2 | �f

	�C�泣����������Ÿ��A�b9562204.c���A�f�]���O2byte���զX�r�A�ҥH��f�N����ܡC
	
	����L�{���|���ͤT���ɮ� out.txt , state.txt �M go_to_table.csv
	
	out.txt��scanner����X�Ӫ�token

	go_to_table.csv�i�H��excel�����}��


	1.

	�sĶ 9562204.c ��A����б��Ѽ� �nŪ���� grammar file �M scanner �nŪ��source code

		e.x >  ./9562204 grammar.txt test.c
	�Y�S���U�o�ⶵ�ѼơA�h�w�]�O�}grammar.txt�Mtest.c�o���file name

	��X�|�b�ù��W�Hpostoder����� parse tree

	�Y�����~�h�|��ܿ��~��state

	2.
	
	����lalr_parser.exe�A���ۦb�������W��file���̿�ܭnŪ����grammar�MC file

	��ܧ�����A���U�e�����ݪ�parsing�A���G�|�H�𪬹���ܦb�e�������C

	�Y�����~�h�|���X���~�����C

	

���ɦC��G

	9562204.c 		:parser
	lalr_parser.exe		:parser with GUI

	test1.c test2.c test3.c test.c :test file
	grammar.c		:�nŪ����grammar
	out.txt			:scanner ������
	lalr_parser		:parser with GUI �� source code ��Borland c++ Builder 2007��project
	