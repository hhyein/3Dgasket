//vertex shader ���: vertex �̵�, lighting
//in: �ҽ� ���� -> vertex shader
//out: vertex shader -> fragment shader

#version 330

in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;
uniform mat4 uMat;

void main()
{
   gl_Position = uMat*vPosition;
   fColor = vColor;
}