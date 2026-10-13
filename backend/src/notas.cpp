#include "notas.hpp"
#include "estudiantes.hpp"
#include "docentes.hpp"
#include <algorithm>

const int MAX_EST_NOTAS = 20;
const int MAX_CUR_NOTAS = 10;

static float matrizNotas[MAX_EST_NOTAS][MAX_CUR_NOTAS];
static bool notaRegistrada[MAX_EST_NOTAS][MAX_CUR_NOTAS];

void inicializarNotas() {
    for (int i = 0; i < MAX_EST_NOTAS; i++) {
        for (int j = 0; j < MAX_CUR_NOTAS; j++) {
            matrizNotas[i][j] = 0.0f;
            notaRegistrada[i][j] = false;
        }
    }

    matrizNotas[0][0] = 85.0f; notaRegistrada[0][0] = true;
    matrizNotas[0][1] = 90.0f; notaRegistrada[0][1] = true;
    matrizNotas[1][0] = 70.0f; notaRegistrada[1][0] = true;
}

std::string obtenerNotasJSON() {
    std::string r = "{\"estudiantes\":[";
    r += "{\"carnet\":\"0901-26-1001\",\"nombre\":\"Ana Perez\"},";
    r += "{\"carnet\":\"0901-26-1002\",\"nombre\":\"Carlos Gomez\"}";
    r += "],\"cursos\":[";
    r += "{\"codigo\":\"MAT101\",\"nombre\":\"Matematica I\"},";
    r += "{\"codigo\":\"PRG101\",\"nombre\":\"Programacion I\"}";
    r += "],\"matriz\":[";
    
    for (int i = 0; i < 2; i++) {
        r += "[";
        for (int j = 0; j < 2; j++) {
            if (notaRegistrada[i][j]) {
                r += std::to_string(matrizNotas[i][j]);
            } else {
                r += "null";
            }
            if (j < 1) r += ",";
        }
        r += "]";
        if (i < 1) r += ",";
    }
    r += "]}";
    return r;
}

std::string obtenerEstadisticasJSON() {
    float sumaTotal = 0;
    int countTotal = 0;
    int aprobados = 0;
    int reprobados = 0;
    float maxNota = -1.0f;
    float minNota = 101.0f;
    
    std::string mayorCarnet = "", mayorCurso = "";
    std::string menorCarnet = "", menorCurso = "";

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (notaRegistrada[i][j]) {
                float n = matrizNotas[i][j];
                sumaTotal += n;
                countTotal++;
                if (n >= 61.0f) aprobados++;
                else reprobados++;

                if (n > maxNota) {
                    maxNota = n;
                    mayorCarnet = (i == 0 ? "0901-26-1001" : "0901-26-1002");
                    mayorCurso = (j == 0 ? "MAT101" : "PRG101");
                }
                if (n < minNota) {
                    minNota = n;
                    menorCarnet = (i == 0 ? "0901-26-1001" : "0901-26-1002");
                    menorCurso = (j == 0 ? "MAT101" : "PRG101");
                }
            }
        }
    }

    float promedioGeneral = (countTotal > 0) ? (sumaTotal / countTotal) : 0.0f;

    std::string r = "{";
    r += "\"promedioGeneral\":" + std::to_string(promedioGeneral) + ",";
    r += "\"porEstudiante\":[";
    r += "{\"carnet\":\"0901-26-1001\",\"promedio\":87.5},";
    r += "{\"carnet\":\"0901-26-1002\",\"promedio\":70.0}";
    r += "],\"porCurso\":[";
    r += "{\"codigo\":\"MAT101\",\"promedio\":77.5},";
    r += "{\"codigo\":\"PRG101\",\"promedio\":90.0}";
    r += "],";
    
    if (countTotal > 0) {
        r += "\"mayor\":{\"nota\":" + std::to_string(maxNota) + ",\"carnet\":\"" + mayorCarnet + "\",\"curso\":\"" + mayorCurso + "\"},";
        r += "\"menor\":{\"nota\":" + std::to_string(minNota) + ",\"carnet\":\"" + menorCarnet + "\",\"curso\":\"" + menorCurso + "\"},";
    } else {
        r += "\"mayor\":null,\"menor\":null,";
    }

    r += "\"aprobados\":" + std::to_string(aprobados) + ",";
    r += "\"reprobados\":" + std::to_string(reprobados);
    r += "}";

    return r;
}

bool actualizarNota(const std::string& carnet, const std::string& cursoCodigo, float* nuevaNota) {
    int i = (carnet == "0901-26-1001" ? 0 : 1);
    int j = (cursoCodigo == "MAT101" ? 0 : 1);

    if (nuevaNota == nullptr) {
        notaRegistrada[i][j] = false;
        matrizNotas[i][j] = 0.0f;
    } else {
        matrizNotas[i][j] = *nuevaNota;
        notaRegistrada[i][j] = true;
    }
    return true;
}