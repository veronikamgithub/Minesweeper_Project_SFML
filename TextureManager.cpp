#include "TextureManager.h"


//redeclare variable so file knows it exists and can use it 
unordered_map<string, sf::Texture> TextureManager::textures;

void TextureManager::LoadTexture(string fileName) {
	string path = "images/";
	path += fileName + ".png";
	textures[fileName].loadFromFile(path);
}

sf::Texture& TextureManager::GetTexture(string textureName) {
	if (textures.find(textureName) == textures.end()){
		LoadTexture(textureName);
	}
	return textures[textureName];
}
void TextureManager::Clear() {
	textures.clear();
}