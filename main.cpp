#include <boost/filesystem.hpp>
#include <boost/crc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>



/*�������� ������ ���������: �������� �������� ������ � ���������, ������� ��������� ����� ��������� �����.
� ���� ��������� ����������� ����������� ����� �����. 
��� ��������� �������� �������� ������� ������� ��������� �����.
��� �������� ����������� ����� ������������ ���������� boost::crc.
*/




int main(int argc, char** argv)
{



	std::string fileCRCName = "NameCRCFile.txt"; // ���� ��� ���������� ����� ����� � ����������� �����
	boost::filesystem::path filePath; // ���� � ����������
	std::cout << "Enter a path: \n";
	std::cin >> filePath;
	std::map<std::string, boost::crc_32_type::value_type> newNameCRC; //������� ��� �������� ����� ������ � ����������� ������
	std::map<std::string, boost::crc_32_type::value_type> oldNameCRC; //������� ��� �������� ����������� ������ � ����������� ������



	if (boost::filesystem::exists(filePath)) // �������� ������������� ����������
	{


		std::ifstream infile(filePath.string() + "\\" + fileCRCName); // ����� ��� ������ �� �����
		if (infile.is_open()) // ���� ������� �������, ������ ��� ����������� ��� �����������, ����� ������� ����������� ������
		{
			std::string name;
			boost::crc_32_type::value_type crc;
			infile >> name;
			while (!infile.eof() && !name.empty())
			{
				infile >> crc;
				oldNameCRC.insert(std::make_pair(name, crc)); // ��������� � �������
				infile >> name;
			}
		}
		else
		{
			std::cout << "The directory " << filePath << " is viewed for the first time.\n";
		}
		infile.close();



		std::for_each(boost::filesystem::directory_iterator(filePath), // ��� ������� ������� � ����������
			boost::filesystem::directory_iterator(),
			[&newNameCRC, &fileCRCName](const boost::filesystem::path& file)
			{
				std::string fileName = file.filename().string();
				if (boost::filesystem::is_regular_file(file) && fileName != fileCRCName) // ���������, ���� �� ���, � ��� �� �� �������� ������ ��� ���������� ����������
				{
					std::ifstream infile(file.string(), std::ios::binary); 
					if (infile.is_open()) // ��������� ��������� ����
					{
						infile.seekg(0, infile.end); //������������ � ����� �����
						std::vector<char> vecChar;
						vecChar.reserve(infile.tellg()); // ����������� � ������� ����������� ���-�� ���������, ����� �������� ����������������� ������
						infile.seekg(0); // ������������ � ������
						char ch;
						while (infile.read(&ch, sizeof(char))) // ��������� ���� ���� �� ������
						{
							vecChar.push_back(ch);
						}
						boost::crc_32_type crc32;
						crc32 = std::for_each(vecChar.begin(), vecChar.end(), crc32); // ������������ ����������� �����
						newNameCRC.insert(std::make_pair(file.filename().string(), crc32.checksum())); // ��������� ��� ����� � ����������� ����� � �������
					}
					infile.close();
				}
		});



		std::ofstream outfile(filePath.string() + "\\" + fileCRCName); // ����� ��� ������ � ���� ����������� ������
		if (outfile.is_open())
		{
			for (auto elem : newNameCRC) outfile << elem.first << " " << elem.second << std::endl; // ���������� � ���� ��� ������ �� �������
		}
		outfile.close();



		if (!oldNameCRC.empty()) // ���� ������� ������ ������ ����, ������ ���������� ����������� ������� � ������� �� � ���
		{
			for (auto elem : newNameCRC)
			{
				auto findelem = oldNameCRC.find(elem.first); // ���� � ������ ������ �� ����� ����� �������� �� ������ �������
				if (findelem != oldNameCRC.end()) // ���� ������� ����� ����
				{
					if (findelem->second != elem.second) // ���������, ���������� �� ����������� �����, ���� �� - ������ ���������
					{
						std::cout << elem.first << " was changed!\n";
					}

				}
				else // ���� ����� ���� �� ������� ����� � ������� ������ ������, ������ �� ��� ������ - ������� ���������
				{
					std::cout << elem.first << " was created!\n";
				}
			}
			for (auto elem : oldNameCRC)
			{
				auto findelem = newNameCRC.find(elem.first); // ���� � ������� ����� ������ ������ �����
				if (findelem == newNameCRC.end()) // ���� ����� �� �������, ������ ���� ��� ������ - ���������
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