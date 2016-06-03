Вариант 6.
ДЗ представляет собой эмулятор файловой системы.

help - вызов справки

Команды разделены на три типа:
1. для работы с пользователями;
2. для работы с директориями и файлами;
3. команды по резервному копировани.


1.
users                              show all users
user                               show current user
user %username%                    change user
2.
dir                                show current directory
cd %dir%                           change current directory to child                
cd ..                              change current directory to parent
mkdir %dir%                        create new child directory
rmdir %dir%                        remove child directory
mkfile %file% %content%            create new file in current directory
rmfile %file%                      remove file from current directory
open %file%                        open file
close %file%                       close file
print %file%                       print file content
edit %file%                        edit file content
chmod %file%                       edit file access
find %file%                        recursive file search from current directory
log %file%                         print log of the file
cpdir %src% %dst%                  copy dirs
cpfile %src% %dst%                 copy files
3.
backups                            list fs backups
backup %name%                      backup current fs
restore %name%                     restore fs from backup


Реализованные процессы:
создание, удаление, просмотр файлов, директорий
поиск файлов(не очень реализовался,правда)
настройка доступа к файлу на чтение и на запись
доступ к файлам обычным пользователем, суперпользователем
смена пользователя
блокирование операций над файлом при его открытии (запрет копирования, пока открыт)
рекурсивная копия директорий
для файла в рамках сессии ведется история доступа (успех, неудача, тип доступа)

Считаю нужным заметить,что на ввод верного пароля у пользователя есть всего три попытки(верные пароли прописаны в тексте программы в комментарии в самом начале).

Пример последовательности команд:
user
111111
mkdir ko
mkfile file1 123456789
edit file(на что будет получен отказ:) )
open file
edit file
close file
log file1
chmod file1 (убирает или выставляет флаг read only)
.



