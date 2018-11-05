#ifndef MESH_H__
#define MESH_H__

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <math.h>
#include "mathutils.h"
#include "texture.h"
#include "model.h"
#include "math.h"
#include "array2d.h"
#include <vector>
#include <map>
#include "textureatlas.h"
#include "resource_manager.h"

class Mesh
{
public:
	Mesh(unsigned int atlasSize = 8);
	~Mesh();
	//void init_instanced_tex_drawing(std::vector<Vertex>* verts, std::vector<GLuint>* inds, std::string tex);
	void add_instanced_base_to_mesh_specific_atlas(std::string model, std::string tex);
	void add_instanced_base_to_mesh_static_atlas(std::string model);
	void AddToMesh(std::vector<Vertex>& verts, const std::vector<GLuint>& inds, int biggestIndex, Vector3f pos, std::string tex, int index = -1);
	void add_tex_offset_static_atlas(std::string tex);
	void add_tex_offset_specific_atlas(std::string tex, int index);
	void change_tex_offset(int pos, std::string tex, int index = -1);
	// Sets basic uv tex_coords (bottom right corner) to the 4 first vertices in m_vertexlist
	void set_placeholder_uv_offset(bool static_atlas);
	void Reset();
	void Finalize(std::string name);
	std::vector<Vertex>* GetMeshVertices();
	std::vector<GLuint>* GetMeshIndices();
	std::vector<Vector2f>* get_tex_coords();
	//std::vector<Transformation> GetMeshModels() { return m_models; };
	TextureAtlas* GetAtlas() { return &m_texAtlas; }
	Vector2f get_uv_offset_coords(std::string tex, int index = -1);

private:
	void add_instanced_base_to_mesh(std::string model, std::string tex, bool use_global_tileset);
	void add_tex_offset(std::string tex, int index = -1);

public:
	bool _instancedDraw;
	bool _tex_drawing;

private:
	std::vector<Vertex> m_vertexList;
	std::vector<GLuint> m_indices;
	std::vector<Vector2f> m_texCoords;
	//std::vector<Transformation> m_models;
	int m_indexProgress;
	TextureAtlas m_texAtlas;
	Texture* m_texture;
	int m_progess;
	std::map<std::string, unsigned int> m_textures;
};

#endif
