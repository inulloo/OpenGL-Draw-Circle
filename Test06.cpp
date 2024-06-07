// Test06.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#ifndef GLAD_IMPLEMENTATION
#define GLAD_IMPLEMENTATION
#include <glad/glad.h>
#endif // GLAD_IMPLEMENTATION

// GLFW_INCLUDE_NONE information
// https://www.glfw.org/docs/latest/build_guide.html
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif // GLFW_INCLUDE_NONE

// ������ѧ����
#include <glm/glm.hpp>
// ��δʹ��
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>

// window size
const int max_window_width = 600;
const int max_window_height = 600;
const int standard_frame_rate = 300;
const GLfloat PI = 3.141592653;// 5897932384626433832795f;
// -------------------------------------------------------------------------------------------------------

// GLFW event callback
// GLFW error
void SetErrorCallback(int error, const char* description);
// resize window
void SetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
// resize window
void SetFramebufferSize(float max_window_width, float max_window_height);
// key in window
void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
// mouse in window
void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// -------------------------------------------------------------------------------------------------------

// ��ɫ�������
void CheckShaderCompileError(unsigned int shader);
void CheckProgramLinkError(unsigned int program);
// -------------------------------------------------------------------------------------------------------

int main()
{
  /* Initialize the library */
  if (glfwInit() == GLFW_FALSE)
  {
    return -1;
  }

  // OpenGL ������־�ص�
  glfwSetErrorCallback(SetErrorCallback);

  // Create a windowed mode window and its OpenGL context
  GLFWwindow* glWindow = glfwCreateWindow(max_window_width, max_window_height, "OpenGL Demo 0.1", NULL, NULL);
  if (glWindow == NULL)
  {
    glfwTerminate();
    return -1;
  }

  // ���ô���ߴ�ı�ص�
  glfwSetFramebufferSizeCallback(glWindow, SetFramebufferSizeCallback);
  // ���ð�ť�¼��ص�
  glfwSetKeyCallback(glWindow, SetKeyCallback);
  // ��갴ť�¼�
  glfwSetMouseButtonCallback(glWindow, SetMouseButtonCallback);
  // Make the window's context current
  glfwMakeContextCurrent(glWindow);

  // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  
  if (!gladLoadGL()) // ���ڼ� gladLoadGLLoader �ĵ���
  {
    return -1;
  }

  // ���贰�ڳߴ�
  SetFramebufferSize(max_window_width, max_window_height);
  // ---------------------------------------------------------------------------

  // ������ɫ�� (Vertex Shader)
  const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\0";
  // Ƭ����ɫ��(Fragment Shader) :
  const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.75f, 0.75f, 0.75f, 1.0f);\n" // ��ɫ
    "}\n\0";

  // ������ɫ��
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
#ifdef _DEBUG
  CheckShaderCompileError(vertexShader);
#endif

  // ������ɫ��
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
#ifdef _DEBUG
  CheckShaderCompileError(fragmentShader);
#endif

  // ������ɫ������
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
#ifdef _DEBUG
  CheckProgramLinkError(shaderProgram);
#endif

  // ɾ����ɫ������
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // ���� VAO �� VBO
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Ϊ VBO �����㹻�Ŀռ䣬���ɴ�Բ��СԲ�ĵ�����
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 300000, NULL, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // �洢Բ�ϵ������
  std::vector<glm::vec2> bigCirclePoints;
  std::vector<glm::vec2> smallCirclePoints;

  // ��Բ�Ƕ�/�뾶
  float bigCircleAngle = 0.0f;
  float bigCircleRadius = 0.6f;
  // СԲ�Ƕ�/�뾶
  float smallCircleAngle = 0.0f;
  float smallCircleRadius = 0.25f;

  // ��ԲСԲ������
  float bigCircleX = 0;
  float bigCircleY = 0;
  float smallCircleX = 0;
  float smallCircleY = 0;
  // ---------------------------------------------------------------------------

  // ��ʱ����ʽ
  double timePrevious = 0;
  double timeLast = 0;
  // ��֡ʱ��
  double frameTimeLimit = 0.01;
  double timeFramePrevious = -1;
  double timeFrameLast = 0;
  // ֡��
  int fps = 0;

  /* �Կ���̨���ж������ */
  // ��ȡ����̨���
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  // ��ȡ����̨��Ļ��������Ϣ
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  // ����Ҫ������е�����
  COORD coord;
  coord.X = 1; // ������ (��0��ʼ)
  coord.Y = 1; // ������ (��0��ʼ)
  // SetConsoleCursorPosition(hConsole, coord);
  // printf("��ǰ֡�ʣ�%d\n", fps);
  // std::cout << "����ַ���: " << fps << "  " << std::flush; // ע�⣺ʹ��flush�������
  // ---------------------------------------------------------------------------

  // ������ϵͳ�����������ù��ĺ�����
  // float fltTick = GetTickCount64() * 0.001;

  // ����֡�ʵĸ���ϵ��
  float fltFactor = 0.1;

  // OpenGL ��ѭ��
  while (!glfwWindowShouldClose(glWindow))
  {
    // ��ȡ��ǰ��ʱ��
    timeFramePrevious = glfwGetTime();
    // ÿ 1 ����һ��֡��
    if ((timeFramePrevious - timeFrameLast) >= 1.0f)
    {
      // �������
      SetConsoleCursorPosition(hConsole, COORD{ 1,3 });
      std::cout << "��ǰ֡��: " << fps << "   " << std::flush;
      SetConsoleCursorPosition(hConsole, COORD{ 1,4 });
      std::cout << "��֡ʱ��: " << frameTimeLimit << "   " << std::flush;

      timeFrameLast = timeFramePrevious;

      // ���ݵ�ǰ֡�ʵ�����֡ʱ�䣬��֡������ standard_frame_rate
      // ���֡�� > standard_frame_rate
      // ��������֡ʱ��
      // ���ӷ�ʽ��A = A + (A * N)
      // ���֡�� < standard_frame_rate
      // �������֡ʱ��
      // ���ٷ�ʽ: A = A - (A * N)
      if (fps > standard_frame_rate)
      {
        if (fps > (standard_frame_rate * 2)) // ʵ��֡�ʴ��� std ����
        {
          fltFactor = 1;
        }
        else if (fps > (standard_frame_rate * 1.5))
        {
          fltFactor = 0.5;
        }
        else if (fps > (standard_frame_rate * 1.1))
        {
          fltFactor = 0.1;
        }
        else
        {
          fltFactor = 0.01;
        }
        frameTimeLimit = frameTimeLimit + (frameTimeLimit * fltFactor);
      }
      // ���ڱ�׼֡��
      else if (fps < standard_frame_rate)
      {
        if (fps < (standard_frame_rate / 1.1))
        {
          fltFactor = 0.1;
        }
        else if (fps < (standard_frame_rate / 1.5))
        {
          fltFactor = 0.5;
        }
        else if (fps < (standard_frame_rate / 2))
        {
          fltFactor = 1;
        }

        frameTimeLimit = frameTimeLimit - (frameTimeLimit * fltFactor);
      }
      fps = 0;

      if (frameTimeLimit < 0.000001)
      {
        frameTimeLimit = 0.1;
      }
    }

    timePrevious = timeFramePrevious;
    // ����֡��ʱ�� ��֡
    if ((timePrevious - timeLast) < frameTimeLimit)
    {
      continue;
    }
    timeLast = timePrevious;
    fps += 1;

    // ---------------------------------------------------------------------------
    // ���ڵ�ɫ
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // �����Բ�µ������
    bigCircleX = bigCircleRadius * cos(glm::radians(bigCircleAngle));
    bigCircleY = bigCircleRadius * sin(glm::radians(bigCircleAngle));
    // ����Բ�µ���ӵ���Բ�ĵ�������
    bigCirclePoints.push_back(glm::vec2(bigCircleX, bigCircleY));

    // ÿ֡СԲ���� i �� ����Բÿ֡���� 1 ��)
    for (int i = 0; i < 4; i++)  // ����A
    {
      if (smallCircleAngle <= 360)
      {
        smallCircleAngle += 0.33;  // ����B
      }
      else
      {
        smallCircleAngle = 0;
      }
      // ����СԲ�µ������ (�Դ�Բ�µ�ΪԲ��)
      float smallCircleX = bigCircleX + smallCircleRadius * cos(glm::radians(smallCircleAngle));
      float smallCircleY = bigCircleY + smallCircleRadius * sin(glm::radians(smallCircleAngle));
      // ��СԲ�µ���ӵ�СԲ�ĵ�������
      smallCirclePoints.push_back(glm::vec2(smallCircleX, smallCircleY));
    }

    // ʹ����ɫ������
    glUseProgram(shaderProgram);

    // ���� VBO ���� (��Բ)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ���� VBO ���� (��Բ)
    glBufferSubData(GL_ARRAY_BUFFER, 0, bigCirclePoints.size() * sizeof(glm::vec2), &bigCirclePoints[0]);

    // ���ƴ�Բ
    glPointSize(1.0f);
    glDrawArrays(GL_POINTS, 0, bigCirclePoints.size());
    // -----------------------------------------------------------------------

    // ���� VBO ���� (СԲ)
    glBufferSubData(GL_ARRAY_BUFFER, bigCirclePoints.size() * sizeof(glm::vec2), smallCirclePoints.size() * sizeof(glm::vec2), &smallCirclePoints[0]);
    // ����СԲ (������ȷ����ʼ����)
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, bigCirclePoints.size(), smallCirclePoints.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ���½Ƕ�
    bigCircleAngle += 0.5f;
    if (bigCircleAngle >= 360.0f)
    {
      bigCircleAngle = 0.0f;
      bigCirclePoints.clear(); //  �����Ҫ���»���Բ���������������
      // smallCirclePoints.clear(); // ���СԲ�ĵ�����
    }

    glfwSwapBuffers(glWindow);
    glfwPollEvents();
  }

  // ������Դ
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwDestroyWindow(glWindow);
  glfwTerminate();

  return 0;
}
// -------------------------------------------------------------------------------------------------------

// �����ɫ���������
void CheckShaderCompileError(unsigned int shader)
{
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
}
// -------------------------------------------------------------------------------------------------------

// �����ɫ���������Ӵ���
void CheckProgramLinkError(unsigned int program)
{
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
}
// -------------------------------------------------------------------------------------------------------

void SetFramebufferSize(float window_width, float window_height)
{
  /*
  // ���ء���������ϵ
  // ��������ͶӰ�������ؽ���д
  // �����ӿ�
  glViewport(0, 0, window_width, window_height);
  // �������� ͶӰ����
  glMatrixMode(GL_PROJECTION);
  // ���ص�λ����
  glLoadIdentity();
  // ��������ͶӰ����, ������Ӿ����ڵ����岻�����ž����Զ�����ı��С�����Ǳ�����ͬ�ı���
  // ���Ͻ���Ϊ 0,0
  // glOrtho(0, window_width, window_height, 0, -1, 1);
  // ���½���Ϊ 0,0
  // glOrtho(0, window_width, 0, window_height, -1, 1);
  // ���ĵ���Ϊ 0,0
  glOrtho(-window_width / 2.0f, window_width / 2.0f, -window_height / 2.0f, window_height / 2.0f, -1.0f, 1.0f);

  // ���õ�ǰ����ģʽΪ ģ����ͼ����
  glMatrixMode(GL_MODELVIEW);
  */

  // Ĭ�ϵ�ͶӰ����ϵ
  // �����ӿڣ������޸ģ������봰�ڴ�Сһ�£�
  glViewport(0, 0, window_width, window_height);

  // ����ͶӰ����
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // ���ĵ�Ϊ����ԭ��
  // glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

  // ����ģ����ͼ����
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
// -------------------------------------------------------------------------------------------------------

// OpenGL �ڲ�������
void SetErrorCallback(int error, const char* description)
{
  printf("%s", description);
}
// -------------------------------------------------------------------------------------------------------

// OpenGL ����ߴ緢���ı�
void SetFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  SetFramebufferSize(width, height);
}
// -------------------------------------------------------------------------------------------------------

// glfw ���������¼������
void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // ���� ESC ��ʱ�رմ���
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
  {

  }

}
// -------------------------------------------------------------------------------------------------------

// ����¼�
void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}
// -------------------------------------------------------------------------------------------------------
