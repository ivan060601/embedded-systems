#ifndef _ERRORS_H_
#define _ERRORS_H_

enum errs{
    NOT_ENOUGHT_ARGS,
    TOO_MANY_ARGS,
    FILE_READ_ERR
};

const char* describe_ru[] =
{
    [NOT_ENOUGHT_ARGS] = "Недостаточно входных параметров для запуска программы\n",
    [TOO_MANY_ARGS] = "Слишком много входных параметров для запуска программы\n",
    [FILE_READ_ERR] = "Ошибка при чтении файла"
};

#endif