�sĶ���ҡG

	windows XP SP3

	1.Dev c++

	2.Borland c++ Builder 2007

�ϥλ����G

	scanner:

	�����A��hw_9562204_scanner.c�A����scanner

	1.����Ū���ɮסA�ѼƱ��n�}�Ҫ��ɮצW��
		E.x >./HW_9562204_scanner test_program.c
	2.�����qstdin��J>./HW_9562204_scanner < test_program.c

	��X���G�|�����C�L�b�ù��W�A�P�ɷ|��Ҧ����G��X���ɮ�out.txt��

	parser:

	grammar file���榡�O

	a.	lefthandside �� righthandside1 | righthandside2 | �f

	�C�泣����������Ÿ��A�b9562204.c���A�f�]���O2byte���զX�r�A�ҥH��f�N����ܡC
	
	����L�{���|���ͨ���ɮ� predict.csv �M table.csv

	�o�Oparse table�A�i�H��excel�����}��


	1.

	�sĶ 9562204.c ��A����б��Ѽ� �nŪ����grammar file �M �n�qscanner Ū�Ӫ�file

		e.x >  ./9562204 grammar.txt out.txt
	�Y�S���U�o�ⶵ�ѼơA�h�w�]�O�}grammar.txt�Mout.txt�o���file name

	��X�|�b�ù��W�Hprefix����� parse tree

	�Y�����~�h�|��ܿ��~��rule

	2.
	
	����project1.exe�A���ۦb�������W��file���̿�ܭnŪ����grammar�Mscan file

	��ܧ�����A���U�e�����ݪ�parsing�A���G�|�H�𪬹���ܦb�e�������C

	�Y�����~�h�|���X���~�����A�æb�������U��ܿ��~��rule

	

���ɦC��G

	9562204.c 		:parser
	HW_9562204_scaner.c	:scanner
	Project1.exe		:parser with GUI

	test1.c test2.c		:test file
	grammar.c		:�nŪ����grammar
	out.txt			:scanner ������
	bcb			:parser with GUI �� source code ��Borland c++ Builder 2007��project
	