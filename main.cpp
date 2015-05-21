#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>



/*Описание работы программы: алгоритм подробно описан в программе, которая проверяет время изменения файла.
В этой программе проверяется контрольная сумма файла. 
Это позволяет избежать проблемы подмены времени изменения файла.
Для рассчета контрольной суммы используется библиотека boost::crc.
*/




int main(int argc, char** argv)
{



	std::string fileCRCName = "NameCRCFile.txt"; // файл для сохранения имени файла и контрольной суммы
	boost::filesystem::path filePath; // путь к директории
	std::cout << "Enter a path: \n";
	std::cin >> filePath;
	std::map<std::string, boost::crc_32_type::value_type> newNameCRC; //словарь для хранения новых данных о контрольных суммах
	std::map<std::string, boost::crc_32_type::value_type> oldNameCRC; //словарь для хранения сохраненных данных о контрольных суммах



	if (boost::filesystem::exists(filePath)) // проверка существования директории
	{


		std::ifstream infile(filePath.string() + "\\" + fileCRCName); // поток для чтения из файла
		if (infile.is_open()) // если удается открыть, значит эта директтория уже проверялась, нужно считать сохраненные данные
		{
			std::string name;
			boost::crc_32_type::value_type crc;
			infile >> name;
			while (!infile.eof() && !name.empty())
			{
				infile >> crc;
				oldNameCRC.insert(std::make_pair(name, crc)); // добавляем в словарь
				infile >> name;
			}
		}
		else
		{
			std::cout << "The directory " << filePath << " is viewed for the first time.\n";
		}
		infile.close();



		std::for_each(boost::filesystem::directory_iterator(filePath), // для каждого объекта в директории
			boost::filesystem::directory_iterator(),
			[&newNameCRC, &fileCRCName](const boost::filesystem::path& file)
			{
				std::string fileName = file.filename().string();
				if (boost::filesystem::is_regular_file(file) && fileName != fileCRCName) // проверяем, файл ли это, и что он не является файлом для сохранения информации
				{
					std::ifstream infile(file.string(), std::ios::binary); 
					if (infile.is_open()) // открываем очередной файл
					{
						infile.seekg(0, infile.end); //перемещаемся в конец файла
						std::vector<char> vecChar;
						vecChar.reserve(infile.tellg()); // резервируем в векторе достаточное кол-во элементов, чтобы избежать перераспределение памяти
						infile.seekg(0); // возвращаемся в начало
						char ch;
						while (infile.read(&ch, sizeof(char))) // считываем файл байт за байтом
						{
							vecChar.push_back(ch);
						}
						boost::crc_32_type crc32;
						crc32 = std::for_each(vecChar.begin(), vecChar.end(), crc32); // рассчитываем контрольную сумму
						newNameCRC.insert(std::make_pair(file.filename().string(), crc32.checksum())); // вставляем имя файла и контрольную сумму в словарь
					}
					infile.close();
				}
		});



		std::ofstream outfile(filePath.string() + "\\" + fileCRCName); // поток для записи в файл обновленных данных
		if (outfile.is_open())
		{
			for (auto elem : newNameCRC) outfile << elem.first << " " << elem.second << std::endl; // записываем в файл все данные из словаря
		}
		outfile.close();



		if (!oldNameCRC.empty()) // если словарь старых данных пуст, значит директория проверялась впервые и сверять не с чем
		{
			for (auto elem : newNameCRC)
			{
				auto findelem = oldNameCRC.find(elem.first); // ищем в старых данных по имени файла элементы из нового словаря
				if (findelem != oldNameCRC.end()) // если удалось найти файл
				{
					if (findelem->second != elem.second) // проверяем, изменилась ли контрольная сумма, если да - вывдим сообщение
					{
						std::cout << elem.first << " was changed!\n";
					}

				}
				else // если новый файл не удалось найти в словаре старых данных, значит он был создан - выводим сообщение
				{
					std::cout << elem.first << " was created!\n";
				}
			}
			for (auto elem : oldNameCRC)
			{
				auto findelem = newNameCRC.find(elem.first); // ищем в словаре новых данных старые файлы
				if (findelem == newNameCRC.end()) // если найти не удалось, значит файл был удален - оповещаем
				{
					std::cout << elem.first << " was deleted!\n";
				}
			}
		}

	}
	else
	{
		std::cout << "Incorrect dir!\n";
	}
	std::cout << "\nDONE!\n";
 	system("pause");
	return 0;
}