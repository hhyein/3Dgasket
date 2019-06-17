//fragment shader 기능: fragment마다 lighting
//in: vertex shader -> fragment shader
//out: fragment shader -> 소스 파일

#version 330

in vec4 fColor;
out vec4 displayColor;

void main()
{
   displayColor = fColor;
}