
#pragma once
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>

constexpr int    MAX_ESTUDIANTES = 100;
constexpr int    MAX_CURSOS      = 15;
constexpr int    DIAS_SEMANA     = 5;        
constexpr float  SIN_NOTA        = -1.0f;   
constexpr double NOTA_APROBACION = 61.0;    
constexpr int    PERIODOS        = 6;       
constexpr int    MAX_CURSOS_POR_BLOQUE = 3;  

struct Estudiante {
    std::string carnet;     
    std::string nombre;
    std::string carrera;
    int semestre = 1;
};

struct CursoEstado {
    std::string codigo;     
    std::string nombre;
    int creditos = 0;
    int semestre = 1;
};

struct Inscripcion {
    std::string carnet;
    std::string curso;       
};

struct Aviso {
    int id = 0;
    std::string titulo;
    std::string descripcion;
    std::string fecha;      
    std::string tipo;       
    std::string curso;      
};

struct Estado {
    std::vector<Estudiante>  estudiantes;
    std::vector<CursoEstado> cursos;
    std::vector<Inscripcion> inscripciones;
    std::vector<Aviso>       avisos;
    int siguiente_id_aviso = 1;

    std::vector<std::string> horario[DIAS_SEMANA][PERIODOS];

    float notas[MAX_ESTUDIANTES][MAX_CURSOS];        
    int   asistencia[MAX_ESTUDIANTES][DIAS_SEMANA]; 
    int   adyacencia[MAX_CURSOS][MAX_CURSOS];       

    std::mutex mtx;  
};

enum class Resultado { OK, DUPLICADO, LLENO, NO_EXISTE };

Estado& estado_global();                        
void inicializar_matrices(Estado& e);           
void cargar_datos_ejemplo(Estado& e);           
void reconstruir_adyacencia(Estado& e);         

int indice_estudiante(const Estado& e, const std::string& carnet);  
int indice_curso(const Estado& e, const std::string& codigo);       
Resultado agregar_estudiante(Estado& e, const Estudiante& nuevo);
Resultado quitar_estudiante(Estado& e, const std::string& carnet);  
Resultado agregar_curso(Estado& e, const CursoEstado& nuevo);
Resultado quitar_curso(Estado& e, const std::string& codigo);      

std::string normalizar_texto(const std::string& texto);             
void ordenar_estudiantes(std::vector<Estudiante>& lista, const std::string& criterio); 
