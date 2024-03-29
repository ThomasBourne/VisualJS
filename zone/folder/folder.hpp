#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

class Folder {
public:
	enum class ItemType {
		Folder,
		File,
		count
	};
	struct Item {
		std::string name;
		ItemType type;
	};

	Folder(std::string path = "C:\\") {
		this->path = path;
	}

	std::vector<Item> GetContents() {
		std::vector<Item> items;
		for (const auto& entry : std::filesystem::directory_iterator(this->path)) {
			items.push_back(Item(entry.path().string(), entry.path().extension() != "" ? ItemType::File : ItemType::Folder));
			//std::cout << entry.path().string() << " : " << entry.path().extension() << std::endl;
		}
		return items;
	}

private:
	std::string path;
};