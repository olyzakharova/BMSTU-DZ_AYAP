// DZv2.cpp: определяет точку входа для консольного приложения.
// сначала публичные методы,потом приватные,потом протектед
//у корня пароль 123456
//у пользователя 111111


#include "stdafx.h"
#include <string>
#include <sstream>// для формирования строк
#include <vector>
#include <iostream>
#include <iomanip> //форматирование вывода
#include <exception> //для обработки исключительных ситуаций (!)
#include <memory>

template<class Entity> //для имен 

Entity* getObjectByName(const std::string& name, std::vector<Entity>& collection)
{
	for (Entity& element : collection)
	{
		if (element.name() == name)
		{
			return &element;
		}
	}

	return nullptr; //если не нашли
}

// метод what возвращает строку,в которой объяснение того,что произошло

class AlreadyExistException : public std::exception //уже существует
{
public:
	AlreadyExistException(const std::string& name) :
		message_("Element '" + name + "' already exist") //элемент уже существует :С
	{ 
	}

	virtual char const* what() const noexcept
	{
		return message_.c_str();
	}

private:
	std::string message_;
};

class DoesntExistException : public std::exception //не существует
{
public:
	DoesntExistException(const std::string& name) :
		message_("Element '" + name + "' doesn't exist")
	{
	}

	virtual char const* what() const noexcept
	{
		return message_.c_str();
	}

private:
	std::string message_;
};

class User
{
public:
	User(const std::string& username, const std::string& password) :
		username_(username),
		password_(password)
	{
	}

	const std::string& name() const
	{
		return username_;
	}

	const std::string& password() const
	{
		return password_;
	}

private:
	std::string username_;
	std::string password_;
};

class FileSystemElement
{
public:
	class BadNameException : public std::exception //плохое имя
	{
	public:
		BadNameException(const std::string& name) :
			message_("Name '" + name + "' is not allowed")
		{
		}

		virtual char const* what() const noexcept
		{
			return message_.c_str();
		}

	private:
		std::string message_;
	};

	const std::string& name() const 
	{
		return name_;
	}

	FileSystemElement* parent() 
	{
		return pParent_;
	}

	const User* owner() const  
	{
		return pOwner_;
	}

	const std::string fullPath() //возвращение полного пути к файлу (используем указатель на предыдущий элемент,идем по родителям)
	{
		std::vector<std::string> parentNames;

		FileSystemElement* currentElement = this;
		while (currentElement)
		{
			parentNames.push_back(currentElement->name_);
			currentElement = currentElement->pParent_;
		}

		std::string path;
		for (auto it = parentNames.crbegin(); it != parentNames.crend(); ++it)
		{
			path += *it;
			path += "/";
		}

		return path;
	}

protected:
	FileSystemElement(const std::string& name, FileSystemElement* pParent, const User* pOwner) :
		name_(name),   
		pParent_(pParent),
		pOwner_(pOwner)
	{
		if (!isNameAllowed(name))
		{
			throw BadNameException(name); // если имя недопустимо
		}
	}

	FileSystemElement() = delete;

	virtual ~FileSystemElement() {}

private:
	static bool isNameAllowed(const std::string& name)
	{
		return name != "..";
	}

protected:
	std::string name_;    //имя
	FileSystemElement* pParent_;   //родительский 
	const User* pOwner_; //создатель (т.к. только создатель файла может его редактировать)
};

class File : public FileSystemElement   // когда файл доступен только для чтения
{
public:
	class ReadOnlyException : public std::exception
	{
	public:
		ReadOnlyException(const std::string& name) : 
			message_("File '" + name + "' is read only")   
		{
		}

		virtual char const* what() const noexcept
		{
			return message_.c_str();
		}

	private:
		std::string message_;
	};

	class OwnerException : public std::exception   //когда пользователь пытается редактировать не свой файл 
	{
	public:
		OwnerException(const std::string& name) :
			message_("Only '" + name + "' can write into this file") 
		{
		}

		virtual char const* what() const noexcept
		{
			return message_.c_str();
		}

	private:
		std::string message_;
	};

	class ClosedException : public std::exception    //закрытые файлы редактировать нельзя С:
	{
	public:
		ClosedException(const std::string& name) :
			message_("File '" + name + "' closed")
		{
		}

		virtual char const* what() const noexcept
		{
			return message_.c_str();
		}

	private:
		std::string message_;
	};

	File(const std::string& name, FileSystemElement* pParent, const User* pOwner, const std::string& content) :
		FileSystemElement(name, pParent, pOwner),
		content_(content),
		pUser_(nullptr),
		readOnly_(false)  
	{
		log_.emplace_back("File was successfully created by user '" + pOwner_->name() + "'");  //по умолчанию в файле можно писать(только powner) //файл создан таким-то пользователем
	}

	const std::string& content() const
	{
		return content_;
	}

	bool readOnly() const
	{
		return readOnly_;
	}

	void setReadOnly(const User* pUser, bool readOnly) 
	{
		if (pUser != pOwner_)
		{
			log_.emplace_back("File belongs to '" + pOwner_->name()
				+ "' but '" + pUser->name() + "' wants to modify it access");//файл принадлежит такому-то пользователю,а другой пользователь пытается его открыть
			throw OwnerException(pOwner_->name());
		}

		log_.emplace_back("File acess was modified by user '" + pUser->name() + "'");

		readOnly_ = readOnly;
	}

	const User* user() const
	{
		return pUser_;
	}

	bool isOpened() const
	{
		return pUser_ != nullptr;
	}

	void open(User* pUser)
	{
		pUser_ = pUser;
	}

	void close()
	{
		pUser_ = nullptr;
	}

	const std::string& read() //файл открыт,если пользователь тру
	{
		if (!isOpened())
		{
			log_.emplace_back("File not opened");
			throw ClosedException(name_);
		}

		log_.emplace_back("File was read by user '" + pUser_->name() + "'"); //файл был прочитан таким-то пользователем

		return content_;
	}

	void write(const User* pUser, const std::string& content)
	{
		if (!isOpened())//если файл не открыт
		{
			log_.emplace_back("File not opened");
			throw ClosedException(name_);
		}

		if (pUser != pOwner_) //если пользователь не владеет файлом
		{
			log_.emplace_back("File belongs to '" + pOwner_->name()
				+ "' but '" + pUser->name() + "' wants to modify it");
			throw OwnerException(pOwner_->name());
		}

		if (readOnly_) //файл только для чтения
		{
			log_.emplace_back("File is read only");
			throw ReadOnlyException(name_);
		}

		log_.emplace_back("File was modified by user '" + pUser_->name() + "'");

		content_ = content;
	}

	const std::vector<std::string>& log() const
	{
		return log_;
	}

private:
	std::string content_; //содержимое файла

	User* pUser_; // пользователь,который редактирует в данный момент

	bool readOnly_; //файл только на чтение

	std::vector<std::string> log_; // учет доступа к файлу
};

class Directory : public FileSystemElement
{
public:
	Directory(const std::string& name, FileSystemElement* pParent, const User* pOwner) :
		FileSystemElement(name, pParent, pOwner)
	{
	}

	static std::unique_ptr<Directory> CreateRoot() //фабричный метод(фабрика) (функция,которая создает объекты и возвращает указатели на них)
	{
		return std::unique_ptr<Directory>(new Directory("", nullptr, nullptr)); //без владельца
	}

	const std::vector<Directory>& dirs() const
	{
		return dirs_;
	}

	const std::vector<File>& files() const
	{
		return files_;
	}

	Directory* getDir(const std::string& name) //получает директорию по имени
	{
		Directory* pDir = getObjectByName(name, dirs_); 

		if (!pDir)
		{
			throw DoesntExistException(name);
		}

		return pDir;
	}

	void createDir(const std::string& name, const User* pOwner) //создание директории
	{
		if (getObjectByName(name, dirs_))
		{
			throw AlreadyExistException(name);  //нельзя две директории с одним именем!
		}

		dirs_.emplace_back(name, this, pOwner); //пихаем директорию в вектор( в кач-ве родителя передаем текущую директорию!!!)
	}

	void removeDir(const std::string& name)  //возвращает имя
	{
		auto it = dirs_.begin();
		for (; it != dirs_.end(); ++it) 
		{
			if (it->name() == name)
			{
				break;
			}
		}

		if (it == dirs_.end())
		{
			throw DoesntExistException(name); //такой директории  нет :С
		}

		dirs_.erase(it); 
	}

	void copyDirContentFromSrc(const Directory* pSrcDir) //в текущую директорию из параметра,который ей передается,копирует содержимое В СЕБЯ	
	{  /* проходит по всем директориям и копирует их в себя,а потом по их содержимому и их в себя */
		for (const Directory& dir : pSrcDir->dirs_)
		{
			const std::string& dstName = dir.name_;
			FileSystemElement* pParent = dir.pParent_;
			const User* pOwner = dir.pOwner_;
			dirs_.emplace_back(dstName, pParent, pOwner);
			Directory* pDstDir = getDir(dstName);

			pDstDir->copyDirContentFromSrc(&dir);
		}

		for (const File& file : pSrcDir->files_)
		{
			createFile(file.name(), file.owner(), file.content());
		}
	}

	void copyDir(const std::string& srcName, const std::string& dstName)
	{
		Directory* pSrcDir = getDir(srcName);
		Directory* pDstDir = nullptr;

		try
		{
			pDstDir = getDir(dstName);
		}
		catch (const DoesntExistException&)
		{
		}

		if (srcName == dstName || pDstDir) //либо передали два одинаковых имени,либо существует директория с дстнейм
		{
			throw AlreadyExistException(dstName);
		}

		FileSystemElement* pParent = pSrcDir->pParent_;
		const User* pOwner = pSrcDir->pOwner_;
		dirs_.emplace_back(dstName, pParent, pOwner);

		pSrcDir = getDir(srcName);
		pDstDir = getDir(dstName);

		pDstDir->copyDirContentFromSrc(pSrcDir);
	}

	File* getFile(const std::string& name) 
	{
		File* pFile = getObjectByName(name, files_);

		if (!pFile)
		{
			throw DoesntExistException(name);
		}

		return pFile;
	}

	void createFile(const std::string& name, const User* pOwner, const std::string& content)
	{
		if (getObjectByName(name, files_))
		{
			throw AlreadyExistException(name);
		}

		files_.emplace_back(name, this, pOwner, content);
	}

	void removeFile(const std::string& name)
	{
		auto it = files_.begin();
		for (; it != files_.end(); ++it)
		{
			if (it->name() == name)
			{
				break;
			}
		}

		if (it == files_.end())
		{
			throw DoesntExistException(name);
		}

		files_.erase(it);
	}

	void copyFile(const std::string& srcName, const std::string& dstName)
	{
		File* pSrcFile = getFile(srcName);

		createFile(dstName, pSrcFile->owner(), pSrcFile->content());
	}

	std::vector<File*> findFile(const std::string& name) //не очень работает
	{
		std::vector<File*> results;

		for (Directory& dir : dirs_)
		{
			std::vector<File*> localResults = dir.findFile(name);
			results.insert(results.end(), localResults.cbegin(), localResults.cend());
		}

		for (File& file : files_)
		{
			if (file.name() == name)
			{
				results.push_back(&file);
			}
		}

		return results;
	}

private:
	std::vector<Directory> dirs_; //список директорий
	std::vector<File> files_;//список файлов
};

class ConsoleEmulator //реализует логику обработки команд и прочее,прочее,прочее
{
public:
	class Command
	{
	public:
		Command(const std::string& command, const std::string& description) :
			command_(command),
			description_(description)
		{
		}

		const std::string& command() const
		{
			return command_;
		}

		const std::string& description() const
		{
			return description_;
		}

	private:
		std::string command_; //команда
		std::string description_; //описание команды
	};

	class Backup //резервная копия
	{
	public:
		Backup(const Directory* pRoot, const std::string& name) :
			pRoot_(Directory::CreateRoot()),
			name_(name)
		{
			pRoot_->copyDirContentFromSrc(pRoot);
		}

		std::unique_ptr<Directory>& root()
		{
			return pRoot_;
		}

		const std::string& name() const
		{
			return name_;
		}

	private:
		std::unique_ptr<Directory> pRoot_; //будет содержать состояние файловой системы
		std::string name_;// нужен,чтоб восстаналиваться потом по этому имени
	};

	ConsoleEmulator() :
		pCurrentUser_(nullptr),
		pRoot_(Directory::CreateRoot()),
		pCurrentDir_(pRoot_.get()),
		exit_(false)
	{
		users_.emplace_back("root", "123456");
		users_.emplace_back("user", "111111");

		userCommands_.emplace_back("users", "show all users"); //команды для работы с пользователями
		userCommands_.emplace_back("user", "show current user");
		userCommands_.emplace_back("user %username%", "change user");

		dirCommands_.emplace_back("dir", "show current directory");//команды,для работы с директориями и файлами
		dirCommands_.emplace_back("cd %dir%", "change current directory to child"); //меняет текущую директорию на дочернюю
		dirCommands_.emplace_back("cd ..", "change current directory to parent"); //подняться на директорию выше
		dirCommands_.emplace_back("mkdir %dir%", "create new child directory");
		dirCommands_.emplace_back("rmdir %dir%", "remove child directory");
		dirCommands_.emplace_back("mkfile %file% %content%", "create new file in current directory");
		dirCommands_.emplace_back("rmfile %file%", "remove file from current directory");
		dirCommands_.emplace_back("open %file%", "open file");
		dirCommands_.emplace_back("close %file%", "close file");
		dirCommands_.emplace_back("print %file%", "print file content");
		dirCommands_.emplace_back("edit %file%", "edit file content");
		dirCommands_.emplace_back("chmod %file%", "edit file access");//поменять права доступа(циклично меняет)
		dirCommands_.emplace_back("find %file%", "recursive file search from current directory");//рекурсивный поиск от текущей директории(не очень работает.но работает)
		dirCommands_.emplace_back("log %file%", "print log of the file");//выводит историю обращений к файлу
		dirCommands_.emplace_back("cpdir %src% %dst%", "copy dirs");//ПОЛНЫЕ ПУТИ НЕ ПОДДЕРЖИВАЮТСЯ
		dirCommands_.emplace_back("cpfile %src% %dst%", "copy files");//ПОЛНЫЕ ПУТИ НЕ ПОДДЕРЖИВАЮТСЯ
																	  //команды по резервному копированию
		backupCommands_.emplace_back("backups", "list fs backups"); //выводит список по доступным копиям
		backupCommands_.emplace_back("backup %name%", "backup current fs");//копировать текущее состояние корня в резервную копию с именем нейм
		backupCommands_.emplace_back("restore %name%", "restore fs from backup");//восстанавливает текущую фс на реервную копию с резервной копией нейм
	}

	void run() //самая клевая функция с:( по крутости вторая после main)
	{
		listAvailableUsers(); 
		login();

		std::string str;
		std::vector<std::string> tokens;

		std::string command;
		std::vector<std::string> commandArgs;

		while (!exit_) //флаг выхода
		{
			std::cout << pCurrentDir_->fullPath() << ": ";

			std::getline(std::cin, str);
			splitString(str, tokens, ' ');
			if (!tokens.empty())
			{
				auto it = tokens.cbegin();
				command = *it++;
				commandArgs.clear();
				for (; it != tokens.cend(); ++it)
				{
					commandArgs.push_back(*it);
				}

				executeCommand(command, commandArgs);
			}
		}
	}

private:
	void help() //спасение с:
	{
		const int commandWidth = 40;
		const int descriptionWidth = 50;

		std::cout << std::setw(commandWidth) << std::left << "Command"
			<< std::setw(descriptionWidth) << std::right << "Description" << std::endl;

		std::cout << std::setw(commandWidth) << std::left << "help"
			<< std::setw(descriptionWidth) << std::right << "print this message" << std::endl;

		std::cout << std::setw(commandWidth) << std::left << "exit"
			<< std::setw(descriptionWidth) << std::right << "exit from program" << std::endl;

		for (const Command& command : userCommands_)
		{
			std::cout << std::setw(commandWidth) << std::left << command.command()
				<< std::setw(descriptionWidth) << std::right << command.description() << std::endl;
		}

		for (const Command& command : dirCommands_)
		{
			std::cout << std::setw(commandWidth) << std::left << command.command()
				<< std::setw(descriptionWidth) << std::right << command.description() << std::endl;
		}

		for (const Command& command : backupCommands_)
		{
			std::cout << std::setw(commandWidth) << std::left << command.command()
				<< std::setw(descriptionWidth) << std::right << command.description() << std::endl;
		}
	}

	void listAvailableUsers() //список доступных пользователей
		//честно выводит имена пользователей
	{
		std::cout << "Available users:" << std::endl;
		for (const User& user : users_)
		{
			std::cout << user.name() << std::endl;
		}
	}

	void login() //пока текущий пользователь не установлен
	{
		while (!pCurrentUser_)
		{
			std::cout << "Username: ";
			std::string username;
			std::getline(std::cin, username);
			User* pUser = getObjectByName(username, users_);
			if (!pUser)
			{
				std::cout << "User '" << username << "' not found" << std::endl;
			}
			else
			{
				authentication(pUser); //попытка идентифицировать
			}
		}
	}

	void authentication(User* pUser) 
	{
		const int maxPasswordAttempts = 3;
		bool authenticated = false;
		for (int i = 0; i < maxPasswordAttempts; ++i)
		{
			std::cout << "Password (" << i + 1 << "/" << maxPasswordAttempts << "): ";
			std::string password;
			std::getline(std::cin, password);
			if (password == pUser->password())
			{
				std::cout << "Successfully authenticated as '" << pUser->name() << "'" << std::endl;
				pCurrentUser_ = pUser;
				authenticated = true;
				break;
			}
		}
		if (!authenticated)
		{
			std::cout << "Authentication failed" << std::endl;
		}
	}

	void listCurrentDir() //текущие директории с вложенными на 1 уровень
	{
		const int nameWidth = 20;
		const int ownerWidth = 20;
		const int acessWidth = 10;
		const int userWidth = 20;

		std::cout << "Directories:" << std::endl;
		std::cout << std::setw(nameWidth) << std::left << "Name"
			<< std::setw(ownerWidth) << std::left << "Owner" << std::endl;
		for (const Directory& dir : pCurrentDir_->dirs())
		{
			std::cout << std::setw(nameWidth) << std::left << dir.name()
				<< std::setw(ownerWidth) << std::left << dir.owner()->name() << std::endl;
		}

		std::cout << "Files:" << std::endl;
		std::cout << std::setw(nameWidth) << std::left << "Name"
			<< std::setw(ownerWidth) << std::left << "Owner"
			<< std::setw(acessWidth) << std::left << "Access"
			<< std::setw(userWidth) << std::left << "User" << std::endl;
		for (const File& file : pCurrentDir_->files())
		{
			std::cout << std::setw(nameWidth) << std::left << file.name()
				<< std::setw(ownerWidth) << std::left << file.owner()->name()
				<< std::setw(acessWidth) << std::left << (file.readOnly() ? "r" : "rw")
				<< std::setw(userWidth) << std::left << ((file.user() != nullptr) ? file.user()->name() : "") << std::endl;
		}
	}

	bool executeUserCommand(const std::string& command, const std::vector<std::string>& commandArgs) //выполнить пользовательскую команду
	{
		if (command == "users")
		{
			listAvailableUsers();
		}
		else if (command == "user")
		{
			if (commandArgs.empty())
			{
				if (!pCurrentUser_)
				{
					std::cout << "Not authenticated" << std::endl;
				}
				else
				{
					std::cout << "Current user: '" << pCurrentUser_->name() << "'" << std::endl;
				}
			}
			else
			{
				const std::string& username = commandArgs.at(0);
				User* pUser = getObjectByName(username, users_);
				if (!pUser)
				{
					std::cout << "User '" << username << "' not found" << std::endl;
				}
				else
				{
					authentication(pUser);
				}
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	bool executeDirCommand(const std::string& command, const std::vector<std::string>& commandArgs)
	{
		try
		{
			if (command == "dir")
			{
				listCurrentDir();
			}
			else if (command == "cd")
			{
				if (!commandArgs.empty())
				{
					const std::string& dirName = commandArgs.at(0);
					if (dirName == "..")
					{
						if (pCurrentDir_ != pRoot_.get())
						{
							pCurrentDir_ = static_cast<Directory*>(pCurrentDir_->parent());
						}
					}
					else
					{
						pCurrentDir_ = pCurrentDir_->getDir(dirName);
					}
				}
			}
			else if (command == "mkdir")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify dir name" << std::endl;
				}
				else
				{
					const std::string& dirName = commandArgs.at(0);
					pCurrentDir_->createDir(dirName, pCurrentUser_);
				}
			}
			else if (command == "rmdir")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify dir name" << std::endl;
				}
				else
				{
					const std::string& dirName = commandArgs.at(0);
					pCurrentDir_->removeDir(dirName);
				}
			}
			else if (command == "mkfile")
			{
				if (commandArgs.size() != 2)
				{
					std::cout << "You need specify file name and content" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					const std::string& fileContent = commandArgs.at(1);
					pCurrentDir_->createFile(fileName, pCurrentUser_, fileContent);
				}
			}
			else if (command == "rmfile")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					pCurrentDir_->removeFile(fileName);
				}
			}
			else if (command == "open")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					pCurrentDir_->getFile(fileName)->open(pCurrentUser_);
				}
			}
			else if (command == "close")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					pCurrentDir_->getFile(fileName)->close();
				}
			}
			else if (command == "print")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					std::cout << "File '" << fileName << "': " << pCurrentDir_->getFile(fileName)->read() << std::endl;
				}
			}
			else if (command == "edit")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					File* pFile = pCurrentDir_->getFile(fileName);
					std::cout << "Old file '" << fileName << "' content: " << pFile->read() << std::endl;
					std::cout << "New file '" << fileName << "' content: ";
					std::string newFileContent;
					std::getline(std::cin, newFileContent);
					pFile->write(pCurrentUser_, newFileContent);;
					std::cout << "Successfully wrote the file '" << fileName << "'" << std::endl;
				}
			}
			else if (command == "chmod")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					File* pFile = pCurrentDir_->getFile(fileName);
					pFile->setReadOnly(pCurrentUser_, !pFile->readOnly());
					std::cout << "File access to '" << fileName << "' has been changed" << std::endl;
				}
			}
			else if (command == "find")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					std::vector<File*> files = pCurrentDir_->findFile(fileName);
					if (files.empty())
					{
						std::cout << "Nothing found" << std::endl;
					}
					else
					{
						std::cout << "Find results:" << std::endl;
						for (File* file : files)
						{
							std::cout << file->fullPath() << std::endl;
						}
					}
				}
			}
			else if (command == "log")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify file name" << std::endl;
				}
				else
				{
					const std::string& fileName = commandArgs.at(0);
					File* pFile = pCurrentDir_->getFile(fileName);
					if (pFile->log().empty())
					{
						std::cout << "File log is empty" << std::endl;
					}
					else
					{
						for (auto str : pFile->log())
						{
							std::cout << str << std::endl;
						}
					}
				}
			}
			else if (command == "cpdir")
			{
				if (commandArgs.size() != 2)
				{
					std::cout << "You need specify src and dst dirs" << std::endl;
				}
				else
				{
					const std::string& srcName = commandArgs.at(0);
					const std::string& dstName = commandArgs.at(1);

					pCurrentDir_->copyDir(srcName, dstName);
				}
			}
			else if (command == "cpfile")
			{
				if (commandArgs.size() != 2)
				{
					std::cout << "You need specify src and dst files" << std::endl;
				}
				else
				{
					const std::string& srcName = commandArgs.at(0);
					const std::string& dstName = commandArgs.at(1);

					pCurrentDir_->copyFile(srcName, dstName);
				}
			}
			else
			{
				return false;
			}
		}
		
		catch (const AlreadyExistException& e)
		{
		    std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const DoesntExistException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const File::ClosedException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const File::ReadOnlyException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const File::OwnerException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const FileSystemElement::BadNameException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}

		return true;
	}

	bool executeBackupCommand(const std::string& command, const std::vector<std::string>& commandArgs)
	{
		try
		{
			if (command == "backups")
			{
				if (backups_.empty())
				{
					std::cout << "No backups" << std::endl;
				}
				else
				{
					std::cout << "Backups:" << std::endl;
					for (Backup& backup : backups_)
					{
						std::cout << backup.name() << std::endl;
					}
				}
			}
			else if (command == "backup")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify backup name" << std::endl;
				}
				else
				{
					const std::string& backupName = commandArgs.at(0);

					createBackup(backupName);
				}
			}
			else if (command == "restore")
			{
				if (commandArgs.empty())
				{
					std::cout << "You need specify backup name" << std::endl;
				}
				else
				{
					const std::string& backupName = commandArgs.at(0);

					restoreBackup(backupName);
				}
			}
			else
			{
				return false;
			}
		}
		catch (const AlreadyExistException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const DoesntExistException& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}

		return true;
	}

	void executeCommand(const std::string& command, const std::vector<std::string>& commandArgs)
	{
		if (command == "help")
		{
			help();
		}
		else if (command == "exit")
		{
			exit_ = true;
		}
		else if (executeUserCommand(command, commandArgs))
		{
			return;
		}
		else if (executeDirCommand(command, commandArgs))
		{
			return;
		}
		else if (executeBackupCommand(command, commandArgs))
		{
			return;
		}
		else
		{
			std::cout << "Unknown command '" << command << "'" << std::endl;
		}
	}

	void createBackup(const std::string& backupName)
	{
		if (getObjectByName(backupName, backups_))
		{
			throw AlreadyExistException(backupName);
		}

		backups_.emplace_back(pRoot_.get(), backupName);
	}

	void restoreBackup(const std::string& backupName)
	{
		Backup* pBackup = getObjectByName(backupName, backups_);

		if (!pBackup)
		{
			throw DoesntExistException(backupName);
		}

		std::unique_ptr<Directory> pRoot = Directory::CreateRoot();
		pRoot->copyDirContentFromSrc(pBackup->root().get());
		pRoot_ = std::move(pRoot);
		pCurrentDir_ = pRoot_.get();
	}

	static void splitString(const std::string& str, std::vector<std::string>& tokens, char delim) //разбивает строку
	{
		std::stringstream ss(str);
		std::string token;
		tokens.clear();
		while (std::getline(ss, token, delim))
		{
			tokens.push_back(token);
		}
	}

private:
	std::vector<User> users_;//пользователи(текущие)

	User* pCurrentUser_; //указатель на текущего пользователя(который залогинен)

	std::unique_ptr<Directory> pRoot_; //текущая фс
	Directory* pCurrentDir_;//текущая директория в текущей фс

	std::vector<Backup> backups_;//бэкапы
	//доступные команды
	std::vector<Command> userCommands_;
	std::vector<Command> dirCommands_;
	std::vector<Command> backupCommands_;

	bool exit_;//флаг выхода
};

int main()
{
	ConsoleEmulator console;
	console.run();

	return 0;
}
