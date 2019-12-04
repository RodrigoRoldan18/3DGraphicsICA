#include "Skybox.h"
#include "MyMesh.h"

void Skybox::Render(const GLuint& argProgram, glm::mat4& argCombined_xform, const glm::mat4& argProjection_xform, const glm::mat4& argView_xform, GLuint& argCombined_xform_id)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	for (MyMesh* mesh : meshVector)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		glUniform1i(glGetUniformLocation(argProgram, "sampler_tex"), 0);

		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->numElements, GL_UNSIGNED_INT, (void*)0);
		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	argCombined_xform = argProjection_xform * argView_xform;
	argCombined_xform_id = glGetUniformLocation(argProgram, "combined_xform");
	glUniformMatrix4fv(argCombined_xform_id, 1, GL_FALSE, glm::value_ptr(argCombined_xform));
}
