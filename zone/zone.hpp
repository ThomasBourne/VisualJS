#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/String.hpp>
#include <fstream>

#include "../FText/FText.hpp"
#include "folder/folder.hpp"

//current problems
//when typing text is not inserted by the cursor but at the end of text

#define CURSOR_PADDING 3
#define CHARACTER_SIZE 30

namespace ZoneInfo {
	std::string transferInfo;
}

//base class
class Zone {
public:
	sf::Vector2u size;
	sf::Vector2u pos;

	bool selected = false;

	Zone(sf::Vector2u size, sf::Vector2u pos, sf::Color colour = sf::Color(30, 30, 30)) {
		this->size = size;
		this->pos = pos;
		this->background = sf::RectangleShape(sf::Vector2f(this->size.x, this->size.y));
		this->background.setPosition(sf::Vector2f(this->pos.x, this->pos.y));
		this->background.setFillColor(colour);

		this->font.loadFromFile("fc.ttf");
	}
	inline bool contains(sf::Vector2f point) {
		return background.getGlobalBounds().contains(point);
	}

	virtual void HandleInput(sf::Event& evnt) { }

	virtual void MoveCursor(sf::Event& evnt) { }

	virtual void Draw(sf::RenderWindow& win) {
		win.draw(this->background);
		if (this->selected) {
			sf::RectangleShape highlight = sf::RectangleShape(sf::Vector2f(this->size.x - 10, this->size.y - 10));
			highlight.setPosition(sf::Vector2f(this->pos.x + 5, this->pos.y + 5));
			highlight.setOutlineColor(sf::Color(100, 30, 30));
			highlight.setOutlineThickness(5);
			highlight.setFillColor(sf::Color(0, 0, 0, 0));
			win.draw(highlight);
		}
	}

	virtual void ReadTransfer(bool& update) {
		ZoneInfo::transferInfo = "";
	}
protected:
	sf::Font font;
	sf::RectangleShape background;
};

//inherited classes
class TextZone: public Zone {
public:
	TextZone(sf::Vector2u size, sf::Vector2u pos, sf::Color colour = sf::Color(30, 30, 30)) : Zone(size, pos, colour) {
		this->size = size;
		this->pos = pos;
		this->background = sf::RectangleShape(sf::Vector2f(this->size.x, this->size.y));
		this->background.setPosition(sf::Vector2f(this->pos.x, this->pos.y));
		this->background.setFillColor(colour);

		this->displayText.setFont(this->font);
		this->displayText.setPosition(sf::Vector2f(pos.x + CHARACTER_SIZE, pos.y + 20));
		this->displayText.setCharacterSize(CHARACTER_SIZE);
		this->displayText.setFillColor(sf::Color(200, 200, 200, 255));

		//set initial cursor
		this->cursorPos = 0;
		this->cursor.setSize(sf::Vector2f(30, 40));
		this->cursor.setPosition(this->displayText.getPosition());
		this->cursor.move(sf::Vector2f(-CURSOR_PADDING, 0));
		this->cursor.setFillColor(sf::Color(200, 200, 200, 20));
	}
	inline bool contains(sf::Vector2f point) {
		return background.getGlobalBounds().contains(point);
	}

	void HandleInput(sf::Event& evnt) { //make sure characters are inserted by the cursor... kinda defeats the whole point of it
		const wchar_t chI = static_cast<wchar_t>(evnt.text.unicode);
		//add a character
		if (std::isprint(chI)) {
			bool moveCursor = this->cursorPos == this->text.getSize();
			this->text += chI;
			this->displayText.setString(this->text);

			if (moveCursor) {
				this->cursorPos++;
				if (this->text.getSize() > 1) {
					this->cursor.move(sf::Vector2f(this->displayText.findCharacterPos(this->cursorPos).x - this->displayText.findCharacterPos(this->cursorPos - 1).x, 0));
				}
			}
			HighlightText();
		}

		//remove a character
		else if (chI == '\b' && this->text.getSize() > 0) {
			if (this->cursorPos == this->text.getSize()) {
				if (this->text.getSize() != 1) {
					this->cursor.move(sf::Vector2f(-(this->displayText.findCharacterPos(this->cursorPos).x - this->displayText.findCharacterPos(this->cursorPos - 1).x), 0));
				}
				this->cursorPos--;
			}

			this->text = this->text.substring(0, this->text.getSize() - 1);
			this->displayText.setString(this->text);
		}
	}

	void MoveCursor(sf::Event& evnt) {
		//move cursor left
		if (this->text.getSize() > 1) {
			if (evnt.key.code == sf::Keyboard::Left && cursorPos > 1) {
				this->cursor.move(sf::Vector2f(-(this->displayText.findCharacterPos(this->cursorPos).x - this->displayText.findCharacterPos(this->cursorPos - 1).x), 0));
				this->cursorPos--;
			}
		}

		//move cursor right
		if (this->text.getSize() > 1) {
			if (evnt.key.code == sf::Keyboard::Right && cursorPos < this->text.getSize()) {
				this->cursorPos++;
				this->cursor.move(sf::Vector2f(this->displayText.findCharacterPos(this->cursorPos).x - this->displayText.findCharacterPos(this->cursorPos - 1).x, 0));
			}
		}
	}

	void Draw(sf::RenderWindow& win) {
		win.draw(this->background);
		this->displayText.setFont(this->font);
		win.draw(this->displayText);
		win.draw(this->cursor);
		if (this->selected) {
			sf::RectangleShape highlight = sf::RectangleShape(sf::Vector2f(this->size.x - 10, this->size.y - 10));
			highlight.setPosition(sf::Vector2f(this->pos.x + 5, this->pos.y + 5));
			highlight.setOutlineColor(sf::Color(100, 30, 30));
			highlight.setOutlineThickness(5);
			highlight.setFillColor(sf::Color(0, 0, 0, 0));
			win.draw(highlight);
		}
	}

	void ReadTransfer(bool& update) {
		std::ifstream test(ZoneInfo::transferInfo);
		if (test)
		{
			std::stringstream buffer;
			buffer << test.rdbuf();
			this->text = buffer.str();
			this->displayText.setString(this->text);
			update = true;
		}
		ZoneInfo::transferInfo = "";
	}
private:
	void HighlightText() { //this messes with cursor actions so needs to be fixed
		/*for (unsigned int i = 0; i < this->text; i++) {
			if (this->text[i] == 'a') {
				this->text.insert(i, "%3");
			}

		}*/
	}
private:
	sf::String text;
	unsigned int cursorPos; //position in sf::String cursor is located (0 based)
	sfe::FText displayText;

	sf::RectangleShape cursor;
};

class FolderZone : public Zone {
public:
	FolderZone(sf::Vector2u size, sf::Vector2u pos, sf::Color colour = sf::Color(30, 30, 30), std::string folderPath = "C:\\") : Zone(size, pos, colour) {
		this->dir = folderPath;

		/*this->text.setString("");
		this->text.setFont(this->font);
		this->text.setPosition(sf::Vector2f(pos.x + 10, pos.y + 20));
		this->text.setCharacterSize(20);
		this->text.setFillColor(sf::Color(200, 200, 200, 255));*/

		this->cursorElement.setSize(sf::Vector2f(0, 0));
		this->cursorElement.setPosition(sf::Vector2f(pos.x + 10, pos.y + 20));
		this->cursorElement.move(sf::Vector2f(-CURSOR_PADDING, 0));
		this->cursorElement.setFillColor(sf::Color(200, 200, 200, 20));
	}

	void HandleInput(sf::Event& evnt) {
		if (evnt.text.unicode == 'o') {
			this->OpenFolder();
		}
		if (evnt.text.unicode == 13) {
			if (this->contents[this->cursorPos].type == Folder::ItemType::Folder) {
				this->dir = std::string(this->contents[this->cursorPos].name + "\\");
				this->OpenFolder();
			}
			else {
				this->cursorElement.setFillColor(sf::Color(200, 200, 50, 100));
				ZoneInfo::transferInfo = this->contents[this->cursorPos].name;
			}
		}
		else if (evnt.text.unicode == '.') {
			//check were not as high as possible
			int count = 0;
			for (size_t offset = this->dir.find('\\'); offset != std::string::npos; offset = this->dir.find('\\', offset + std::string("\\").length())) {
				++count;
			}
			if (count > 1) { //remove last bit of directory;
				this->dir = this->dir.substr(0, this->dir.find_last_of('\\'));
				this->dir = this->dir.substr(0, this->dir.find_last_of('\\') + 1);
				this->OpenFolder();
			}
		}
	}

	void MoveCursor(sf::Event& evnt) {
		if (this->cursorElement.getSize().x > 0) {
			//move cursor up
			if (evnt.key.code == sf::Keyboard::Up && this->cursorPos > 0) {
				this->cursorElement.setPosition(this->text[this->cursorPos].getPosition());
				this->cursorElement.setSize(sf::Vector2f(this->cursorElement.getSize().x, this->text[this->cursorPos].getGlobalBounds().height));
				this->cursorPos--;
			}

			//move cursor down
			if (evnt.key.code == sf::Keyboard::Down && this->cursorPos < this->text.size()-1) {
				this->cursorElement.setPosition(this->text[this->cursorPos+1].getPosition());
				this->cursorElement.setSize(sf::Vector2f(this->cursorElement.getSize().x, this->text[this->cursorPos + 1].getGlobalBounds().height));
				this->cursorPos++;
			}
		}
	}

	void Draw(sf::RenderWindow& win) {
		win.draw(this->background);
		for (sfe::FText t : this->text)
			win.draw(t);
		win.draw(this->cursorElement);
		if (this->selected) {
			sf::RectangleShape highlight = sf::RectangleShape(sf::Vector2f(this->size.x - 10, this->size.y - 10));
			highlight.setPosition(sf::Vector2f(this->pos.x + 5, this->pos.y + 5));
			highlight.setOutlineColor(sf::Color(100, 30, 30));
			highlight.setOutlineThickness(5);
			highlight.setFillColor(sf::Color(0, 0, 0, 0));
			win.draw(highlight);
		}
	}

private:
	void OpenFolder() {
		this->contents.clear();
		this->text.clear();
		Folder f{ this->dir };
		std::vector<Folder::Item> items = f.GetContents();
		for (int i = 0; i < items.size(); i++) {
			Folder::Item& it = items[i];
			std::string displayString = it.name;
			if (displayString.length() > 23) {
				displayString.insert(23, "\n");
			}
			std::string tmp = std::string("%") + std::string(it.type == Folder::ItemType::File ? "1" : "5") + displayString;
			this->contents.push_back(it);

			sfe::FText text;
			text.setString(tmp);
			text.setFont(this->font);
			text.setPosition(sf::Vector2f(pos.x + 10, pos.y + 20));
			text.setCharacterSize(20);
			text.setFillColor(sf::Color(200, 200, 200, 255));
			if (!this->text.empty())
				text.move(sf::Vector2f(0, this->text[this->text.size() - 1].getGlobalBounds().height + this->text[this->text.size() - 1].getGlobalBounds().top - 20));
			this->text.push_back(text);
		}

		this->cursorPos = 0;
		this->cursorElement.setPosition(sf::Vector2f(pos.x + 10, pos.y + 20));
		if (!this->contents.empty()) {
			this->cursorElement.setSize(sf::Vector2f(285, 25));
		}
		else {
			this->cursorElement.setSize(sf::Vector2f(0, 0));
		}
	}
private:
	std::string dir;
	std::vector<Folder::Item> contents;
	std::vector<sfe::FText> text;

	unsigned int cursorPos;
	sf::RectangleShape cursorElement;
};

class HintZone : public Zone {
public:
	HintZone(sf::Vector2u size, sf::Vector2u pos, sf::Color colour = sf::Color(30, 30, 30)) : Zone(size, pos, colour) {
		this->hints.setFont(this->font);
		this->hints.setPosition(sf::Vector2f(pos.x + CHARACTER_SIZE, pos.y + 20));
		this->hints.setCharacterSize(22);
		this->hints.setFillColor(sf::Color(200, 200, 200, 255));
		this->hints.setString(sf::String("Hints:\nCtrl+Scrll: zoom\nCtrl+Q: reset view\n. for updir\nEntr for dir\nEntr for file\n%0Format%2ted %3text"));
	}

	void HandleInput(sf::Event& evnt) { }

	void Draw(sf::RenderWindow& win) {
		win.draw(this->background);
		this->hints.setFont(this->font);
		win.draw(this->hints);
		if (this->selected) {
			sf::RectangleShape highlight = sf::RectangleShape(sf::Vector2f(this->size.x - 10, this->size.y - 10));
			highlight.setPosition(sf::Vector2f(this->pos.x + 5, this->pos.y + 5));
			highlight.setOutlineColor(sf::Color(100, 30, 30));
			highlight.setOutlineThickness(5);
			highlight.setFillColor(sf::Color(0, 0, 0, 0));
			win.draw(highlight);
		}
	}
private:
	sfe::FText hints;
};
