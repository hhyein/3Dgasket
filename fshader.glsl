//fragment shader ���: fragment���� lighting
//in: vertex shader -> fragment shader
//out: fragment shader -> �ҽ� ����

#version 330

in vec4 fColor;
out vec4 displayColor;

void main()
{
   displayColor = fColor;
}