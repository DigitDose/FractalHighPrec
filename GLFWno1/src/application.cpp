#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

using namespace std;

const double PI = 3.14159265358979323846;
float offsetX = 0.0f;
float offsetY= 0.0f;
float zoomlvl = 1.1f;
float zwiek_it = 1.0f;
float zoom_it = 0;
const int max_zoom_it = 100;

class HighPrec 
{
private: 
    vector<int> liczby;
    int poz_punkt_dz;

public:
    HighPrec(const string& numer) 
    {
        poz_punkt_dz = numer.find('.');
        if (poz_punkt_dz != string::npos) {
            string czesc_cal = numer.substr(0, poz_punkt_dz);
            string czesc_ul = numer.substr(poz_punkt_dz + 1);

            for (const char& c : czesc_cal) {
                liczby.push_back(c - '0');
            }
            for (const char& c : czesc_ul) {
                liczby.push_back(c - '0');
            }
        }
        else {
            poz_punkt_dz = numer.length();
            for (const char& c : numer) {
                liczby.push_back(c - '0');
            }
        }

    }

};
void przyb(GLFWwindow* window, double/* xoffset */, double yoffset)
{
    if (yoffset > 0)
    {
        zoomlvl *= 1.1f;
    }
    else if (yoffset < 0)
    {
        zoomlvl /= 1.1f;
    }
    
    if (zoom_it >= max_zoom_it) {
        offsetX *= zoom_it;
        offsetY *= zoom_it;
        zoomlvl = 1.1f;
        zoom_it = 0;
    }
}


void klaw(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float offset_in = 0.05f*zoomlvl;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) 
    {
        switch (key) {
            case GLFW_KEY_1:
                zwiek_it *= 1.1f;
                break;
            case GLFW_KEY_2:
                zwiek_it /= 1.1f;
                break;
            case GLFW_KEY_LEFT:
                offsetX += offset_in;
                break;
            case GLFW_KEY_RIGHT:
                offsetX -= offset_in;
                break;
            case GLFW_KEY_DOWN:
                offsetY += offset_in;
                break;
            case GLFW_KEY_UP:
                offsetY -= offset_in;
                break;
            case GLFW_KEY_A:
                offsetX += offset_in;
                break;
            case GLFW_KEY_D:
                offsetX -= offset_in;
                break;
            case GLFW_KEY_S:
                offsetY += offset_in;
                break;
            case GLFW_KEY_W:
                offsetY -= offset_in;
                break;
            default:
                break;
        }

    }
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)

    {   
        char* message1 = new char;
        int lenght;
        glGetShaderInfoLog(id, GL_INFO_LOG_LENGTH, &lenght, message1);
        char* message = (char*)malloc(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "FAILED TO COMPILE " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "shader " << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        delete message1;
        message = nullptr;
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
void gen_frakt(const glm::vec2& poz, glm::vec3& color)
{
    const int max_iter = 1000;
    glm::vec2 liczb_r = poz;
    glm::vec2 liczb_u = glm::vec2(0.0f);
    int iter = 0;

    while (glm::length(liczb_u) < 2.0f && iter < max_iter)
    {
        liczb_u = glm::vec2(liczb_u.x * liczb_u.x - liczb_u.y * liczb_u.y, 2.0f * liczb_u.x * liczb_u.y) + liczb_r;
        iter++;
    }
    if (iter < max_iter)
    {
        float norm_it = static_cast<float>(iter) / static_cast<float>(max_iter);
        color = glm::vec3(
            0.5f + 0.5f * cos(3.0f + norm_it * PI),
            0.5f + 0.5f * cos(3.0f + norm_it * 2.0f * PI),
            0.5f + 0.5f * cos(3.0f + norm_it * 4.0f * PI)

        );
    }
    else {
        color = glm::vec3(0.0f, 0.0f, 0.0f);
    }
}
glm::vec3 samplepix(const glm::vec2& poz) 
{
    glm::vec3 sum_kol(0.0f);
    int samples = 16;

    for (int i = 0; i < samples; i++) {
        glm::vec2 random_offset(
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX
        );

        glm::vec2 sample_poz = poz + random_offset;
        glm::vec3 sample_kol;
        gen_frakt(sample_poz, sample_kol);
        sum_kol += sample_kol;
    }
    return sum_kol / static_cast<float>(samples);

}

int main(void)
{
    GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    

    /* TWORZENIE OKNA I KONTEKSTU OPENGL */
    glfwWindowHint(GLFW_SAMPLES, 16);
    window = glfwCreateWindow(1920, 1080, "OKNO GL", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, klaw);
    glfwSetScrollCallback(window, przyb);

    
    // FUNKCJA glewInit() musi byæ wywo³ana po poprawnie stworzonym kontekœcie
    if (glewInit() != GLEW_OK) 
    {
        std::cout << "nie powiod³a siê inicjalizacja" << std::endl;
    }
 /* przyk³ad który pokazuje ¿e mo¿na ko¿ystaæ z funkcji jako wskaŸnik//
        unsigned int a;                                              //
        glGenBuffers(1, &a);                                         //
        std::cout << glGetString(GL_VERSION) << std::endl;           //
 ////////////////////////////////////////////////////////////////////*/
   

    std::string vertexShaderSource = R"(
    #version 440 core
    layout(location = 0) in vec2 a_position;

    

    void main() 
    {
        gl_Position = vec4(a_position, 0.0, 1.0);
    }
)";
    std::string fragmentShaderSource = R"(
    #version 440 core
    layout(location = 0) out vec4 color;

    uniform vec2 offset;
    uniform highp float zoomlvl;
    uniform highp float zwiek;

    void main()
    {
        
      highp float max_iterations = 1000.0/zwiek; 
      //   double max_iterations = 1000.0/zwiek; 

       
        vec2 c = ((gl_FragCoord.xy * 2.0 - vec2(1920, 1080)) / min(1920, 1080)*zoomlvl)- offset*0.5;
        vec2 z = vec2(0.0);
        highp float iterations = 0.0;

        while (length(z) < 1.5 && iterations < max_iterations) {
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
            iterations++;
        }

        if (iterations < max_iterations) {
            highp float normalized_iterations = iterations / max_iterations;
            color = vec4(
                0.5 + 0.5 * cos(3.0 + normalized_iterations * 3.1416),
                0.5 + 0.5 * cos(0.5 + normalized_iterations * 2.0 * 3.1416),
                0.5 + 0.5 * cos(1.0 + normalized_iterations * 4.0 * 3.1416),
                1.0);
        } else {
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }
)";
    unsigned int shader = CreateShader(vertexShaderSource,fragmentShaderSource);
    int offsetLocation = glGetUniformLocation(shader, "offset");
    int zoomLocation = glGetUniformLocation(shader, "zoomlvl");
    int zwiekLocation = glGetUniformLocation(shader, "zwiek");

    //definicja tablicy wierzcho³ków (4) typu float
    float wierzch[] = {

        -1.0f,-1.0f,
         1.0f,-1.0f,
        -1.0f, 1.0f,
         1.0f, 1.0f,
    };
    //stworzenie VBO  iprzekazanie mu danych
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wierzch), wierzch, GL_STATIC_DRAW);
    //stworzenie VCO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glEnable(GL_MULTISAMPLE);



    while (!glfwWindowShouldClose(window))
    {   

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shader);
        /*
        glUniform2d(offsetLocation, offsetX, offsetY);
        glUniform1d(zoomLocation, zoomlvl);
        glUniform1d(zwiekLocation, zwiek_it);
        */
        glUniform2f(offsetLocation, offsetX, offsetY);
        glUniform1f(zoomLocation, zoomlvl);
        glUniform1f(zwiekLocation, zwiek_it);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);  // renderowanie kwadratu za pomoc¹ dwóch trójk¹tów
       
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
   
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}