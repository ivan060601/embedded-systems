#ifndef _ERRORS_H_
#define _ERRORS_H_

enum errs{
    NOT_ENOUGHT_ARGS,
    TOO_MANY_ARGS,
    FILE_READ_ERR
};

const char* describe_ru[] =
{
    [NOT_ENOUGHT_ARGS] = "������������ ������� ���������� ��� ������� ���������\n",
    [TOO_MANY_ARGS] = "������� ����� ������� ���������� ��� ������� ���������\n",
    [FILE_READ_ERR] = "������ ��� ������ �����"
};

#endif