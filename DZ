#include "stdafx.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

void split(const string& str, char delim, vector<string>& tokens)
{
	stringstream ss(str);
	string token;
	tokens.clear();
	while (getline(ss, token, delim))
	{
		tokens.push_back(token);
	}
}

class User
{
public:
	User(string name, string password) : name_(name), password_(password) {}

	string& name()
	{
		return name_;
	}

	const string& name() const
	{
		return name_;
	}

	string& password()
	{
		return password_;
	}

	const string& password() const
	{
		return password_;
	}

	bool askPassword()
	{
		string password;

		for (int i = 0; i < 3; ++i)
		{
			cout << "password (" << i + 1 << "/" << 3 << "): ";
			getline(cin, password);

			if (password == password_)
			{
				return true;
			}
		}

		return false;
	}

private:
	string name_;
	string password_;
};

vector<User> users = { User("root", "password"), User("user", "111111") };
User* currentUser = &users[1];

class File
{
public:
	File(const string& name, const string& content) : name_(name), content_(content), user_() {}

	string& name()
	{
		return name_;
	}

	const string& name() const
	{
		return name_;
	}

	string& content()
	{
		return content_;
	}

	const string& content() const
	{
		return content_;
	}

	string& user()
	{
		return user_;
	}

	const string& user() const
	{
		return user_;
	}

	void open()
	{
		if (!user_.empty() && user_ != currentUser->name())
		{
			cout << "file is opened by another user ('" << user_ << "')" << endl;
		}
		else
		{
			user_ = currentUser->name();
		}
	}

	void close()
	{
		if (!user_.empty() && user_ != currentUser->name())
		{
			cout << "file is opened by another user ('" << user_ << "')" << endl;
		}
		else
		{
			user_ = "";
		}
	}

	void show()
	{
		if (user_.empty())
		{
			cout << "you must open file before show its content" << endl;
		}
		else
		{
			if (user_ != currentUser->name())
			{
				cout << "file is opened by another user ('" << user_ << "')" << endl;
			}
			else
			{
				cout << content_ << endl;
			}
		}
	}

private:
	string name_;
	string content_;
	string user_;
};

class Directory
{
public:
	Directory() : parentDir_(nullptr), name_(""), dirs_(), files_() {}
	Directory(Directory* parentDir, const string& name) : parentDir_(parentDir), name_(name), dirs_(), files_() {}

	Directory* parentDir()
	{
		return parentDir_;
	}

	const Directory* parentDir() const
	{
		return parentDir_;
	}

	string& name()
	{
		return name_;
	}

	const string& name() const
	{
		return name_;
	}

	string path()
	{
		vector<string> names;
		Directory* currentDir = this;
		while (currentDir)
		{
			names.push_back(currentDir->name_);
			currentDir = currentDir->parentDir_;
		}

		string path;
		for (auto it = names.rbegin(); it != names.rend(); ++it)
		{
			path += *it;
			path += "/";
		}

		return path;
	}

	void displayDir()
	{
		if (dirs_.empty() && files_.empty())
		{
			cout << "directory is empty" << endl;
		}
		else
		{
			cout << setw(25) << left << "name" << setw(10) << left << "type" << "user" << endl;
			for (const Directory& dir : dirs_)
			{
				cout << setw(25) << left << dir.name_ << "dir" << endl;
			}
			for (const File& file : files_)
			{
				cout << setw(25) << left << file.name() << setw(10) << left << "file" << file.user() << endl;
			}
		}
	}

	Directory* getChildDir(const string& name)
	{
		for (Directory& dir : dirs_)
		{
			if (dir.name_ == name)
			{
				return &dir;
			}
		}

		return nullptr;
	}

	void createChildDir(const string& name)
	{
		if (getChildDir(name) != nullptr)
		{
			cout << "directory already exist" << endl;
		}
		else
		{
			dirs_.emplace_back(Directory(this, name));
		}
	}

	void removeChildDir(const string& name)
	{
		dirs_.erase(remove_if(dirs_.begin(), dirs_.end(), [&name](const Directory& dir) { return dir.name_ == name; }));
	}

	File* getFile(const string& name)
	{
		for (File& file : files_)
		{
			if (file.name() == name)
			{
				return &file;
			}
		}

		return nullptr;
	}

	void createFile(const string& name, const string& content)
	{
		if (getFile(name) != nullptr)
		{
			cout << "file '" << name << "' already exist" << endl;
		}
		else
		{
			files_.emplace_back(File(name, content));
		}
	}

	void removeFile(const string& name)
	{
		files_.erase(remove_if(files_.begin(), files_.end(), [&name](const File& file) { return file.name() == name; }));
	}

	void doCommand(const string& command, const vector<string>& args)
	{
		if (command == "dir")
		{
			displayDir();
		}
		else if (command == "mkdir")
		{
			if (args.size() != 1)
			{
				cout << "wrong arguments. usage: mkdir %dir_name%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					createChildDir(args[0]);
				}
			}
		}
		else if (command == "rmdir")
		{
			if (args.size() != 1)
			{
				cout << "wrong arguments. usage: rmdir %dir_name%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					removeChildDir(args[0]);
				}
			}
		}
		else if (command == "mkfile")
		{
			if (args.size() == 0)
			{
				cout << "wrong arguments. usage: mkfile %file_name% %content%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					createFile(args[0], args.size() == 1 ? string() : args[1]);
				}
			}
		}
		else if (command == "rmfile")
		{
			if (args.size() == 0)
			{
				cout << "wrong arguments. usage: rmfile %file_name%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					removeFile(args[0]);
				}
			}
		}
		else if (command == "open")
		{
			if (args.size() == 0)
			{
				cout << "wrong arguments. usage: open %file_name%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					File* file = getFile(args[0]);
					if (file == nullptr)
					{
						cout << "file '" << args[0] << "' not exist" << endl;
					}
					else
					{
						file->open();
					}
				}
			}
		}
		else if (command == "close")
		{
			if (args.size() == 0)
			{
				cout << "wrong arguments. usage: close %file_name%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					File* file = getFile(args[0]);
					if (file == nullptr)
					{
						cout << "file '" << args[0] << "' not exist" << endl;
					}
					else
					{
						file->close();
					}
				}
			}
		}
		else if (command == "show")
		{
			if (args.size() == 0)
			{
				cout << "wrong arguments. usage: show %file_name%" << endl;
			}
			else
			{
				if (args[0] == "..")
				{
					cout << "symbol '..' is reserved. try another name" << endl;
				}
				else
				{
					File* file = getFile(args[0]);
					if (file == nullptr)
					{
						cout << "file '" << args[0] << "' not exist" << endl;
					}
					else
					{
						file->show();
					}
				}
			}
		}
		else
		{
			cout << "unknown command: " << command << endl;
		}
	}

private:
	Directory* parentDir_;
	string name_;
	vector<Directory> dirs_;
	vector<File> files_;
};

void displayHelp()

{
	cout << "  dir                          - show current directory" << endl
		<< "  mkdir %dir_name%             - create new directory in current directory" << endl
		<< "  rmdir %dir_name%             - remove directory from current directory" << endl
		<< "  mkfile %file_name% %content% - create new file in current directory with selected content (may be empty)" << endl
		<< "  rmfile %file_name%           - remove file from current directory" << endl
		<< "  open %file_name%             - open file under current user" << endl
		<< "  close %file_name%            - close file under current user" << endl
		<< "  show %file_name%             - show file content" << endl
		<< "  user                         - show current user" << endl
		<< "  user %user_name%             - change current user to %user_name%" << endl
		<< "  help                         - show this message" << endl
		<< "  exit                         - exit from program" << endl;
}

int main(int argc, char* argv[])
{
	Directory root;
	Directory* currentDir = &root;

	string str;
	vector<string> tokens;
	string command;
	vector<string> args;

	cout << "type 'exit' to finish the program" << endl << endl;

	for (;;)
	{
		cout << currentDir->path() << ": ";
		getline(cin, str);

		if (str == "exit")
		{
			break;
		}
		else if (str == "help")
		{
			displayHelp();
		}
		else
		{
			split(str, ' ', tokens);
			if (!tokens.empty())
			{
				auto it = tokens.cbegin();
				command = *it;
				++it;
				args.clear();
				for (; it != tokens.cend(); ++it)
				{
					args.push_back(*it);
				}

				if (command == "user")
				{
					if (args.empty())
					{
						cout << "current user: " << currentUser->name() << endl;
					}
					else if (args.size() == 1)
					{
						bool userFound = false;

						for (User& user : users)
						{
							if (user.name() == args[0])
							{
								userFound = true;
								if (user.askPassword())
								{
									currentUser = &user;
									cout << "login success" << endl;
								}
								else
								{
									cout << "login failed" << endl;
								}
								break;
							}
						}

						if (!userFound)
						{
							cout << "user '" << args[0] << "' not found" << endl;
						}
					}
					else
					{
						cout << "wrong arguments. usage: user or user %user_name%" << endl;
					}
				}
				else if (command == "cd")
				{
					if (args.size() == 1)
					{
						if (args[0] == "..")
						{
							if (currentDir->parentDir() != nullptr)
							{
								currentDir = currentDir->parentDir();
							}
						}
						else
						{
							Directory* childDir = currentDir->getChildDir(args[0]);
							if (childDir == nullptr)
							{
								cout << "there are no child directory with name '" << args[0] << "'" << endl;
							}
							else
							{
								currentDir = childDir;
							}
						}
					}
					else
					{
						cout << "wrong arguments. usage: cd %dir_name%" << endl;
					}
				}
				else
				{
					currentDir->doCommand(command, args);
				}
			}
		}
		cout << endl;
	}

	return 0;
}

