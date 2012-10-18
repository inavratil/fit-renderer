/**
@file utils.hpp
@brief various utilities, mostly from OpenGL forums
***************************************************************************************************/
#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "globals.h"
#include <algorithm>
#include <iomanip>

/**
****************************************************************************************************
@brief Print uniform block info. Courtesy of "wien"
http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=287747#Post287747
***************************************************************************************************/
void print_uniform_block_info(GLuint prog, GLint block_index, std::string const &indent = std::string())
{
	// Fetch uniform block name:
	GLint name_length;
	glGetActiveUniformBlockiv(prog, block_index, GL_UNIFORM_BLOCK_NAME_LENGTH, &name_length);
	std::string block_name(name_length, 0);
	glGetActiveUniformBlockName(prog, block_index, name_length, NULL, &block_name[0]);

	// Fetch info on each active uniform:
	GLint active_uniforms = 0;
	glGetActiveUniformBlockiv(prog, block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &active_uniforms);

	std::vector<GLuint> uniform_indices(active_uniforms, 0);
	glGetActiveUniformBlockiv(prog, block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, reinterpret_cast<GLint*>(&uniform_indices[0]));

	std::vector<GLint> name_lengths(uniform_indices.size(), 0);
	glGetActiveUniformsiv(prog, uniform_indices.size(), &uniform_indices[0], GL_UNIFORM_NAME_LENGTH, &name_lengths[0]);

	std::vector<GLint> offsets(uniform_indices.size(), 0);
	glGetActiveUniformsiv(prog, uniform_indices.size(), &uniform_indices[0], GL_UNIFORM_OFFSET, &offsets[0]);

	std::vector<GLint> types(uniform_indices.size(), 0);
	glGetActiveUniformsiv(prog, uniform_indices.size(), &uniform_indices[0], GL_UNIFORM_TYPE, &types[0]);
	
	std::vector<GLint> sizes(uniform_indices.size(), 0);
	glGetActiveUniformsiv(prog, uniform_indices.size(), &uniform_indices[0], GL_UNIFORM_SIZE, &sizes[0]);

	std::vector<GLint> strides(uniform_indices.size(), 0);
	glGetActiveUniformsiv(prog, uniform_indices.size(), &uniform_indices[0], GL_UNIFORM_ARRAY_STRIDE, &strides[0]);

	// Build a string detailing each uniform in the block:
	std::vector<std::string> uniform_details;
	uniform_details.reserve(uniform_indices.size());
	for(std::size_t i = 0; i < uniform_indices.size(); ++i)
	{
		GLuint const uniform_index = uniform_indices[i];

		std::string name(name_lengths[i], 0);
		glGetActiveUniformName(prog, uniform_index, name_lengths[i], NULL, &name[0]);

		std::ostringstream details;
		details << std::setfill('0') << std::setw(4) << offsets[i] << ": " << std::setfill(' ') << std::setw(5) << types[i] << " " << name;

		if(sizes[i] > 1)
		{
			details << "[" << sizes[i] << "]";
		}

		details << "\n";
		uniform_details.push_back(details.str());
	}

	// Sort uniform detail string alphabetically. (Since the detail strings 
	// start with the uniform's byte offset, this will order the uniforms in 
	// the order they are laid out in memory:
	std::sort(uniform_details.begin(), uniform_details.end());

	// Output details:
	std::cout << indent << "Uniform block \"" << block_name << "\":\n";
    for(std::vector<std::string>::iterator detail = uniform_details.begin(); detail != uniform_details.end(); ++detail)
	{
		std::cout << indent << "  " << *detail;
	}
}

#endif
