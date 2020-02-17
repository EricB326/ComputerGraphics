#include "Mesh.h"

/* .OBJ importing includes
*/
#define TINYOBJLOADER_IMPLEMENTATION
#include "..\Dependencies\tiny_obj\tiny_obj_loader.h"

#include <chrono>

namespace uciniti
{
	Mesh::Mesh()
		: m_VAO(0), m_VBO(0), m_EBO(0), m_indices(0), m_index_count(0), m_vert_count(0), m_empty_mesh(true)
	{
	}

	Mesh::Mesh(const char* a_filepath, bool a_load_textures, bool a_flip_textures)
		: m_VAO(0), m_VBO(0), m_EBO(0), m_indices(0), m_index_count(0), m_vert_count(0), m_empty_mesh(true)
	{
		load_obj(a_filepath, a_load_textures, a_flip_textures);
	}

	Mesh::Mesh(GLfloat* a_vertices, uint* a_indices, uint a_num_of_verts, uint a_num_of_indices, vertex_type a_vertex_type)
		: m_VAO(0), m_VBO(0), m_EBO(0), m_indices(a_indices), m_index_count(a_num_of_indices), m_vert_count(a_num_of_verts), m_empty_mesh(true)
	{
		// Check to see if the vertex type is invalid
		if ((uint)a_vertex_type < 0 || (uint)a_vertex_type > 1)
		{
			std::cout << "Failed to construct mesh! Invalid vertex typed passed!\n";
			return;
		}

		// Check what vetex struct is being populated
		switch (a_vertex_type)
		{
			case vertex_type::BASE_VERTEX:
			{
				// Loop through the passed vertice array, jump by 6 spaces each time to return
				// to the "start" of each new "segment" of data example:
				// verts[] = { 3, 0, -2, 0.5f, 0.5f, 0.5f,   <-- Stats at "3", reads all the data on that line to the final "0.5f"
				//			  -3, 0, 2, 1.0f, 1.0f, 1.0f };  <-- Jumped += 6 positions to "-3" and reads all data to final "1.0f"
				for (size_t i = 0; i < m_vert_count; i += 6)
				{
					// Create a tempory vertex holder for pushing into the the <vector>
					auto vert = base_vertex();

					// Read all the data on that "segment" of the array
					vert.m_position = glm::vec3(a_vertices[i], a_vertices[i + 1], a_vertices[i + 2]);
					vert.m_colour = glm::vec3(a_vertices[i + 3], a_vertices[i + 4], a_vertices[i + 5]);

					// Pust the data into the <vertor> storage
					m_base_vert.push_back(vert);
				}

				// Setup the meshes ID's
				setup_base_mesh();
				break;
			}
			case vertex_type::STANDARD_VERTEX:
			{
				// Read above lines: 16 -> 19. Instead of jumping by 6, jump by 18.
				for (size_t i = 0; i < m_vert_count; i += 18)
				{
					// Create a tempory vertex holder for pushing into the the <vector>
					auto vert = standard_vertex();

					// Read all the data on that "segment" of the array
					vert.m_position = glm::vec4(a_vertices[i], a_vertices[i + 1], a_vertices[i + 2], a_vertices[i + 3]);
					vert.m_normal = glm::vec4(a_vertices[i + 4], a_vertices[i + 5], a_vertices[i + 6], a_vertices[i + 7]);

					vert.m_tangent = glm::vec4(a_vertices[i + 8], a_vertices[i + 9], a_vertices[i + 10], a_vertices[i + 11]);
					vert.m_bitangent = glm::vec4(a_vertices[i + 12], a_vertices[i + 13], a_vertices[i + 14], a_vertices[i + 15]);

					vert.m_tex_coords = glm::vec2(a_vertices[i + 16], a_vertices[i + 17]);

					// Pust the data into the <vertor> storage
					m_standard_vert.push_back(vert);
				}

				// Setup the meshes ID's
				setup_standard_mesh();
				break;
			}
			case vertex_type::FULL_VERTEX:
			{
				// Read above lines: 16 -> 19. Instead of jumping by 6, jump by 19.
				for (size_t i = 0; i < m_vert_count; i += 19)
				{
					// Create a tempory vertex holder for pushing into the the <vector>
					auto vert = full_vertex();

					// Read all the data on that "segment" of the array
					vert.m_position = glm::vec3(a_vertices[i], a_vertices[i + 1], a_vertices[i + 2]);
					vert.m_colour = glm::vec3(a_vertices[i + 3], a_vertices[i + 4], a_vertices[i + 5]);

					vert.m_normal = glm::vec3(a_vertices[i + 6], a_vertices[i + 7], a_vertices[i + 8]);
					vert.m_tangent = glm::vec4(a_vertices[i + 9], a_vertices[i + 10], a_vertices[i + 11], a_vertices[i + 12]);

					vert.m_tex_coords001 = glm::vec2(a_vertices[i + 13], a_vertices[i + 14]);
					vert.m_tex_coords002 = glm::vec2(a_vertices[i + 15], a_vertices[i + 16]);
					vert.m_tex_coords003 = glm::vec2(a_vertices[i + 17], a_vertices[i + 18]);

					// Pust the data into the <vertor> storage
					m_full_vert.push_back(vert);
				}

				// Setup the meshes ID's
				setup_full_mesh();
				break;
			}
			default:
			{
				printf("No vertex type detected!\n");
				return;
			}
		}
	}

	void Mesh::render_mesh()
	{
		// Error checking for none existant IDs
		if (!m_VAO)
		{
			printf("VAO ID does not exist!\n");
			return;
		}

		if (!m_VBO)
		{
			printf("VBO ID does not exist!\n");
			return;
		}

		// Specify the VAO and EBO we are working with
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

		// Check if the program should draw using indicies
		if (m_EBO)
			glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, 3 * m_index_count);

		// Unbind the VAO and EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::render()
	{
		render_mesh();
	}

	void Mesh::draw()
	{
		render_mesh();
	}

	bool Mesh::load_obj(const char* a_filepath, bool a_load_textures, bool a_filp_textures_v)
	{
		// Check to see if the mesh already has a model loaded.
		if (!m_empty_mesh)
		{
			clear_mesh();
		}
		// Store the seperate objects and their faces.
		std::vector<tinyobj::shape_t> shapes;
		// Materials and textures per face.
		std::vector<tinyobj::material_t> materials;
		// Contain errors that occur while loading files.
		std::string err;

		std::string file = a_filepath;
		std::string folder = file.substr(0, file.find_last_of('/') + 1);
		
		// Check current time point.
		//auto measure_time_start = std::chrono::high_resolution_clock::now();
		//printf("Reading '%s'... ", a_filepath);
		if (!tinyobj::LoadObj(shapes, materials, err, a_filepath, folder.c_str()))
		{
			printf("\nERROR: load_obj() call. Failed to load file '%s'!.\n", a_filepath);
			printf("%s\n", err.c_str());
			return false;
		}

		// Pass data to the correct struct class.
		for (const auto& shape : shapes)
		{
			// Set the size vertex vector, as well as check how many verts there are.
			m_standard_vert.resize(shape.mesh.positions.size() / 3);
			size_t vert_count = m_standard_vert.size();

			// Copy indice data into member variables.
			m_index_count = shape.mesh.indices.size();
			m_indices = shape.mesh.indices.data();

			printf("\ntt..%i\n", shape.mesh.indices[72]);
			printf("tt..%i\n", m_indices[72]);

			printf("\nIndex count..%i\n", m_index_count);

			// Populate the vertex.
			for (size_t i = 0; i < vert_count; i++)
			{
				// If there is mesh positional data, populate the position vec3.
				if (!shape.mesh.positions.empty())
					m_standard_vert[i].m_position = glm::vec4(shape.mesh.positions[i * 3], shape.mesh.positions[i * 3 + 1], shape.mesh.positions[i * 3 + 2], 1.0f);
				// If there is mesh normals data, populate the normal vec3.
				if (!shape.mesh.normals.empty())
					m_standard_vert[i].m_normal = glm::vec4(shape.mesh.normals[i * 3], shape.mesh.normals[i * 3 + 1], shape.mesh.normals[i * 3 + 2], 0.0f);
				// Invert the texture if the texture requires it.
				if (!shape.mesh.texcoords.empty())
					m_standard_vert[i].m_tex_coords = glm::vec2(shape.mesh.texcoords[i * 2 + 0], a_filp_textures_v ? 1.0f - shape.mesh.texcoords[i * 2 + 1] : shape.mesh.texcoords[i * 2 + 1]);
			}

			// Calculate tangents for normal mapping.
			if (!shape.mesh.normals.empty() && !shape.mesh.texcoords.empty())
				calculate_vertex_tangents(m_standard_vert, m_indices);

			//printf("post calculate tangents\n");
		}

		// Prepare the mesh for rendering.
		setup_standard_mesh();

		return true;
	}

	void Mesh::calculate_vertex_tangents(std::vector<standard_vertex>& a_vertices, const uint* a_indices)
	{
		/* Reference:
		   Title: Foundations of Game Engine Development, Volume 2: Rendering
		   Author: Eric Lengyel
		   Date: 2019
		   Availability: http://foundationsofgameenginedev.com/
		*/

		// Store the amount of vertices.
		uint vertex_count = (uint)a_vertices.size();
		// Allocate storage for the tangents and bitangents.
		glm::vec4* tangent = new glm::vec4[vertex_count * 2];
		glm::vec4* bitangent = tangent + vertex_count;
		// Initialize the tangent and bitangent vectors to zero.
		for (size_t i = 0; i < vertex_count; i++)
		{
			tangent[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			bitangent[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		// Calculate tangent and bitangent for each triangle.
		for (size_t i = 0; i < m_index_count  - 2; i++)
		{
			long index0 = a_indices[i];
			long index1 = a_indices[i + 1];
			long index2 = a_indices[i + 2];

			const glm::vec3& point0 = a_vertices[index0].m_position;
			const glm::vec3& point1 = a_vertices[index1].m_position;
			const glm::vec3& point2 = a_vertices[index2].m_position;

			const glm::vec2& w0 = a_vertices[index0].m_tex_coords;
			const glm::vec2& w1 = a_vertices[index1].m_tex_coords;
			const glm::vec2& w2 = a_vertices[index2].m_tex_coords;

			glm::vec3 e1 = point1 - point0, e2 = point2 - point0;
			float x1 = w1.x - w0.x, x2 = w2.x - w0.x;
			float y1 = w1.y - w0.y, y2 = w2.y - w0.y;

			float r = 1.0f / (x1 * y2 - x2 * y1);
			glm::vec3 t = (e1 * y2 - e2 * y1) * r;
			glm::vec3 b = (e2 * x1 - e1 * x2) * r;

			tangent[index0] += glm::vec4(t, 0.0f);
			tangent[index2] += glm::vec4(t, 0.0f);
			tangent[index1] += glm::vec4(t, 0.0f);

			bitangent[index0] += glm::vec4(b, 0.0f);
			bitangent[index2] += glm::vec4(b, 0.0f);
			bitangent[index1] += glm::vec4(b, 0.0f);
		}
		printf("calc\n");
		// Orthonormalize each tanget and calculate handedness.
		for (size_t i = 0; i < vertex_count; i++)
		{
			const glm::vec3& t = tangent[i];
			const glm::vec3& b = bitangent[i];
			const glm::vec3& n = a_vertices[i].m_normal;

			// Gram-Schmidt orthogonalize.
			a_vertices[i].m_tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 0.0f);

			// Calculate handedness (direction of bitangent).
			a_vertices[i].m_tangent.w = (glm::dot(glm::cross(t, b), n) > 0.0f) ? 1.0f : -1.0f;

			// Calculate bitangent.
			a_vertices[i].m_bitangent = glm::vec4(glm::cross(n, glm::vec3(a_vertices[i].m_tangent)) * a_vertices[i].m_tangent.w, 0.0f);
			a_vertices[i].m_tangent.w = 0;
		}

		// Clean memory
		delete[] tangent;
	}

	void Mesh::setup_base_mesh()
	{
		// Create the vertex array ID and bind it
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Create the vertex buffer ID and bind it
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		// Attach the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(base_vertex) * m_base_vert.size(), &m_base_vert[0], GL_STATIC_DRAW);

		// Create EBO buffer ID and bind it
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		// Attach the indices data to the EBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_index_count, &m_indices[0], GL_STATIC_DRAW);

		// Create and enable the attribute for the position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(base_vertex), (GLvoid*)0);
		// Create and enable the attribute for the colours
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(base_vertex), (GLvoid*)offsetof(base_vertex, m_colour));

		// Unbind the VBO, EBO, and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::setup_standard_mesh()
	{
		// Create the vertex array ID and bind it
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Create the vertex buffer ID and bind it
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		// Attach the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(standard_vertex) * m_standard_vert.size(), &m_standard_vert[0], GL_STATIC_DRAW);

		// Create EBO buffer ID and bind it
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		// Attach the indices data to the EBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_index_count, &m_indices[0], GL_STATIC_DRAW);

		// Create and enable the attribute for the position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(standard_vertex), (GLvoid*)0);
		// Create and enable the attribute for the normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(standard_vertex), (GLvoid*)offsetof(standard_vertex, m_normal));
		// Create and enable the attribute for the tangents
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(standard_vertex), (GLvoid*)offsetof(standard_vertex, m_tangent));
		// Create and enable the attribute for the bitangents
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(standard_vertex), (GLvoid*)offsetof(standard_vertex, m_bitangent));
		// Create and enable the attribute for the texels (U/V)
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(standard_vertex), (GLvoid*)offsetof(standard_vertex, m_tex_coords));

		// Unbind the VBO, EBO, and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::setup_full_mesh()
	{
		// Create the vertex array ID and bind it
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Create the vertex buffer ID and bind it
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		// Attach the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(full_vertex) * m_full_vert.size(), &m_full_vert[0], GL_STATIC_DRAW);

		// Create EBO buffer ID and bind it
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		// Attach the indices data to the EBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_index_count, &m_indices[0], GL_STATIC_DRAW);

		// Create and enable the attribute for the position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)0);
		// Create and enable the attribute for the colour
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)offsetof(full_vertex, m_colour));
		// Create and enable the attribute for the normal
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)offsetof(full_vertex, m_normal));
		// Create and enable the attribute for the tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)offsetof(full_vertex, m_tangent));
		// Create and enable the attribute for the texels (U/V) 001
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)offsetof(full_vertex, m_tex_coords001));
		// Create and enable the attribute for the texels (U/V) 002
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)offsetof(full_vertex, m_tex_coords002));
		// Create and enable the attribute for the texels (U/V) 003
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(full_vertex), (GLvoid*)offsetof(full_vertex, m_tex_coords003));

		// Unbind the VBO, EBO, and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::clear_mesh()
	{
		// Clear all the buffers off the graphics card and reset the IDs
		if (m_VAO)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}

		if (m_VBO)
		{
			glDeleteBuffers(1, &m_VBO);
			m_VBO = 0;
		}

		if (m_EBO)
		{
			glDeleteBuffers(1, &m_EBO);
			m_EBO = 0;
		}

		// Reset the index count
		m_index_count = 0;
	}
}