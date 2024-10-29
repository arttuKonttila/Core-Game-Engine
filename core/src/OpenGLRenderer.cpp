#include "../include/OpenGLRenderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = nullptr;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = nullptr;

PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUNIFORM2FPROC glUniform2f = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLDETACHSHADERPROC glDetachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLUNIFORM1IVPROC glUniform1iv = nullptr;
PFNGLUNIFORM2IVPROC glUniform2iv = nullptr;
PFNGLUNIFORM3IVPROC glUniform3iv = nullptr;
PFNGLUNIFORM4IVPROC glUniform4iv = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM1FVPROC glUniform1fv = nullptr;
PFNGLUNIFORM2FVPROC glUniform2fv = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = nullptr;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv = nullptr;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv = nullptr;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv = nullptr;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = nullptr;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = nullptr;

// Shader
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;

// VBO
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC	glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC	glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = nullptr;

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = nullptr;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = nullptr;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = nullptr;
PFNGLCLEARDEPTHFPROC glClearDepthf = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;

#include "../include/GL/wglext.h"
PFNGLBLENDEQUATIONPROC glBlendEquation = nullptr;
PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2D glCompressedTexImage2D = nullptr;
PFNGLBLENDCOLORPROC glBlendColor = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;

OpenGLRenderer::OpenGLRenderer() :
	m_Context(nullptr),
	m_RC(nullptr)
{
}

OpenGLRenderer::~OpenGLRenderer()
{
	if (m_Context)
	{
		wglMakeCurrent(m_Context, nullptr);
		wglDeleteContext(m_RC);
		::ReleaseDC(IApplication::GetApp()->GetWindow(), m_Context);
		m_Context = nullptr;
	}
}

bool OpenGLRenderer::Create()
{
	m_Context = ::GetDC(IApplication::GetApp()->GetWindow());
	uint32_t flags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		flags,
		PFD_TYPE_RGBA,	// kind of framebuffer, RGBA or palette.
		32,				// colordepth of the framebuffer
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,				// depth buffer bits
		8,				// stencil buffer bits
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixelFormat = ::ChoosePixelFormat(m_Context, &pfd);
	if (pixelFormat == 0)
	{
		return false;
	}
	if (!::SetPixelFormat(m_Context, pixelFormat, &pfd))
	{
		return false;
	}

	m_RC = wglCreateContext(m_Context);
	wglMakeCurrent(m_Context, m_RC);

	InitFunctions();

	SetDefaultSettings();

	return true;
}

void OpenGLRenderer::Flip()
{
	glFlush();
	SwapBuffers(m_Context);
}

void OpenGLRenderer::Clear(float r, float g, float b, float a, float depth, int32_t stencil)
{
	glClearDepthf(depth);
	glClearStencil(stencil);

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderer::SetViewport(const glm::ivec4& area)
{
	glViewport(area.x, area.y, area.z, area.w);
}

bool OpenGLRenderer::SetTexture(uint32_t program, uint32_t texture, uint32_t slot, const std::string_view& uniformName)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture);
	const GLint location = glGetUniformLocation(program, uniformName.data());
	if (location >= 0) {
		glUniform1i(location, slot);
		return true;
	}
	return false;
}

bool OpenGLRenderer::SetDefaultSettings()
{
	glStencilMask(0);
	glDisable(GL_SCISSOR_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_FUNC);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_STENCIL_TEST);

	glClearDepthf(1.0f);
	glClearStencil(0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_CCW);

	return true;
}

GLuint OpenGLRenderer::CreateTexture(const std::string_view& filename)
{
	GLuint textureHandle = 0;
	int32_t texturewidth = 0;
	int32_t textureHeight = 0;
	int32_t bpp = 0;
	uint8_t* imgdata = stbi_load(filename.data(), &texturewidth, &textureHeight, &bpp, STBI_rgb_alpha);
	if (!imgdata || !texturewidth || !textureHeight || !bpp) {
		// failed to load image file
		IApplication::Debug("Failed to load image");
		IApplication::Debug(filename.data());
		return 0;
	}

	// premultiply alpha
	const int32_t imgdatabytes = texturewidth * textureHeight * 4;
	for (int32_t i = 0; i < imgdatabytes; i += 4) {
		const int32_t alpha = imgdata[i + 3];
		if (alpha != 255) {
			imgdata[i] = imgdata[i] * alpha / 255;
			imgdata[i + 1] = imgdata[i + 1] * alpha / 255;
			imgdata[i + 2] = imgdata[i + 2] * alpha / 255;
		}
	}

	GLint internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;
	GLenum err = glGetError();

	glGenTextures(1, &textureHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		internalFormat,
		texturewidth,
		textureHeight,
		0,
		format,
		GL_UNSIGNED_BYTE,
		imgdata);

	delete[] imgdata;

	err = glGetError();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureHandle;
}

GLuint OpenGLRenderer::CreateVertexShader(const char* sourceCode)
{
	GLuint shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderHandle, 1, (const char**)&sourceCode, nullptr);
	glCompileShader(shaderHandle);

	GLint shaderCompiled = 0;

	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderCompiled);

	if (!shaderCompiled) {
		IApplication::Debug("Failed to compile vertex shader");
		PrintShaderError(shaderHandle);
		glDeleteShader(shaderHandle);
		shaderHandle = 0;
	}

	return shaderHandle;
}

GLuint OpenGLRenderer::CreateVertexShaderFromFile(const std::string_view& filename)
{
	std::ifstream f(filename.data(), std::ios::binary);
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(f)),
		(std::istreambuf_iterator<char>())
	);
	bytes.push_back(0);

	return CreateVertexShader(bytes.data());
}

GLuint OpenGLRenderer::CreateFragmentShader(const char* sourceCode)
{
	GLuint shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderHandle, 1, (const char**)&sourceCode, nullptr);
	glCompileShader(shaderHandle);

	GLint shaderCompiled = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderCompiled);

	if (!shaderCompiled) {
		IApplication::Debug("Failed to compile fragment shader");
		PrintShaderError(shaderHandle);
		glDeleteShader(shaderHandle);
		shaderHandle = 0;
	}

	return shaderHandle;
}

GLuint OpenGLRenderer::CreateFragmentShaderFromFile(const std::string_view& filename)
{
	std::ifstream f(filename.data(), std::ios::binary);
	std::vector<char> bytes(
		(std::istreambuf_iterator<char>(f)),
		(std::istreambuf_iterator<char>())
	);
	bytes.push_back(0);

	return CreateFragmentShader(bytes.data());
}

GLuint OpenGLRenderer::CreateProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint programHandle = glCreateProgram();
	glAttachShader(programHandle, fragmentShader);
	glAttachShader(programHandle, vertexShader);
	glLinkProgram(programHandle);

	GLint linked = 0;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
	if (!linked) {
		IApplication::Debug("Failed to link program: ");
		PrintProgramError(programHandle);
		glDeleteProgram(programHandle);
		programHandle = 0;
	}
	return programHandle;
}

void OpenGLRenderer::PrintShaderError(GLuint shader)
{
	GLint infologLength = 0;
	GLint charsWritten = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength) {
		char* infoLog = new char[infologLength + 1];
		memset(infoLog, 0, infologLength + 1);
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		IApplication::Debug(infoLog);
		delete[] infoLog;
	}
}

void OpenGLRenderer::PrintProgramError(GLuint program)
{
	GLint infologLength = 0;
	GLint charsWritten = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength) {
		char* infoLog = new char[infologLength + 1];
		memset(infoLog, 0, infologLength + 1);
		glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
		IApplication::Debug(infoLog);
		delete[] infoLog;
	}
}

bool OpenGLRenderer::InitFunctions()
{
#if defined (_WINDOWS)
#define GL_GETPROCADDRESS wglGetProcAddress
#define GL_GETPROCADDRESS_PARAM_TYPE const char*
#endif
#if defined (_LINUX)
#define GL_GETPROCADDRESS glXGetProcAddressARB
#define GL_GETPROCADDRESS_PARAM_TYPE const GLubyte*
#endif

#if defined (_WINDOWS)
	glBlendEquation = (PFNGLBLENDEQUATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendEquation");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glActiveTexture");
	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2D)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCompressedTexImage2D");
	glBlendColor = (PFNGLBLENDCOLORPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendColor");
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
#endif

	glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendEquationSeparate");
	glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBlendFuncSeparate");

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetProgramInfoLog");

	glUniform2f = (PFNGLUNIFORM2FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform2f");
	glUniform3f = (PFNGLUNIFORM3FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform4f");

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteBuffers");

	glBindBuffer = (PFNGLBINDBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindBuffer");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniform4fv");
	glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniformMatrix3fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetActiveUniform");

	// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGetShaderiv");

	// VBO

	glGenBuffers = (PFNGLGENBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenBuffers");
	glBufferData = (PFNGLBUFFERDATAPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttribPointer");
	glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glVertexAttribIPointer");

	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenFramebuffers");
	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenRenderbuffers");
	glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteFramebuffers");
	glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glDeleteRenderbuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindFramebuffer");
	glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glBindRenderbuffer");
	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glRenderbufferStorage");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glFramebufferTexture2D");
	glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glFramebufferTexture");
	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glFramebufferRenderbuffer");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glCheckFramebufferStatus");
	glClearDepthf = (PFNGLCLEARDEPTHFPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glClearDepthf");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)GL_GETPROCADDRESS((GL_GETPROCADDRESS_PARAM_TYPE)"glGenerateMipmap");

	// check that functions were loaded properly
	if (!glCreateProgram)
	{
		IApplication::Debug("Renderer_OpenGL::InitFunctions - failed to find required OpenGL functions. Most likely there is no valid OpenGL drivers installed");
		return false;
	}

	return true;
}