### Task & Status (in Russian)

**Текущий итог:**\
**200 *баллов***

На языке Си не используя внешних зависимостей написать программу, которая:

- [x] Открывает TCP/IP сервер на любом порту (*+25 баллов*)
- [x] Ожидает подключение не более чем от 1 клиента одновременно (*+5 баллов*)
- [x] В случае, если порт уже занят другой программой сообщает об этом (*+5 баллов*)
- [x] Получает от клиента строку, оканчивающуся символами \r\n (*+10 баллов*)
- [x] Строка может быть любой длины (*+10 баллов*)
- [x] После получения интерпретирует строку как математическое выражение (*+20 баллов*)\
*\<S>\<A>\<S>\<O>\<S>\<B>\<S>*\
где *\<S>* - любое количество пробелов, включая 0\
*\<A>* - первое число в пределах INT32_MIN..INT32_MAX\
*\<B>* - второе число в пределах INT32_MIN..INT32_MAX\
*\<O>* - математический или логический оператор (+, -, *, / и т.д)\
допускается многосимвольная комбинация, например "&&"
- [x] Программа поддерживает не менее 5 операторов (*+5 баллов*)
- [x] Программа поддерживает не менее 10 операторов (*+10 баллов*)
- [x] Программа поддерживает не менее 15 операторов (*+15 баллов*)
- [x] Программа обрабатывает случаи переполнения INT32 и выдает текстовое сообщение, а не неверный результат (*+10 баллов*)
- [x] Результат отправляется по сокету обратно клиенту в текстовом виде (*+20 баллов*)
- [x] Сервер закрывает соединение (*+5 баллов*)\
Исходный код сервера не показывает проблем или предупреждений в:
- [x] cppcheck (*+10 баллов*)
- [x] clang-analyzer (*+10 баллов*)
- [x] pvs-studio (*+20 баллов*)
- [x] Исходный код сервера должен находиться на git-сервере (*+5 баллов*)\
Любой на выбор. И иметь не менее 5 коммитов и 2 веток в истории.
- [x] Программа имеет make-файл с опциями <programname>, clean и all (*+15 баллов*)

**Супербуст (*необязательно для выполнения*)**:
- [ ] Программа поддерживает любое количество операторов в строке и скобки (подсказка: перевод в постфиксную систему) (*+100 баллов*)
- [ ] Все функции, глобальные переменные, константы и файлы имеют встроенную документацию в формате doxygen (*+50 баллов*)\
Makefile должен иметь опцию doc для сборки документации в html формате.

Для прохождения тестового задания нужно набрать **не менее 150 баллов**.

В качестве тестового клиента можно использовать встроенную команду telnet.\
Рекомендуется использовать форматирование кода, соответствующее следующим правилам программы indent: `indent -kr -brf`

**Пример работы программы**:\
<Запуск сервера>\
<Запуск клиента>\
Клиент посылает: "55+21\r\n"\
Сервер посылает клиенту "76\r\n"\
Сервер закрывает соединение
