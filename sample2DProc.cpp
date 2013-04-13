
MaterialPtr mat =  m_material_manager->Create("glsl_computeAliasError"); // new Material
mat->SetCustomShaders("render.vert", "render,frag");
mat->addTexture(texCoords);


[...]

GLuint fboid = app->getPass("2D_computeAliasError")->getFBO();
GLuint texid = app->getPass("2D_computeAliasError")->getTexture("tex_output");

[...]

void Application::PrerenderPass()
{
	for(PassIterator it_pass; it_pass!=end(); it_pass++)
	{
		PassPtr pass = it_pass;
		pass->Render();

	}
}
