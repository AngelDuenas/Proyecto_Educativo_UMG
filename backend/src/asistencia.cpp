#include "asistencia.hpp"

const int MAX_EST_ASIST = 20;
const int NUM_DIAS = 5;

static int matrizAsistencia[MAX_EST_ASIST][NUM_DIAS];
static int totalEstudiantesAsist = 2;

void inicializarAsistencia() {
    for (int i = 0; i < MAX_EST_ASIST; i++) {
        for (int j = 0; j < NUM_DIAS; j++) {
            matrizAsistencia[i][j] = 1;
        }
    }
    matrizAsistencia[0][2] = 0;
}

std::string obtenerAsistenciaJSON() {
    std::string r = "{\"dias\":[\"L\",\"M\",\"X\",\"J\",\"V\"],";
    r += "\"estudiantes\":[";
    r += "{\"carnet\":\"0901-26-1001\",\"nombre\":\"Ana Perez\"},";
    r += "{\"carnet\":\"0901-26-1002\",\"nombre\":\"Carlos Gomez\"}";
    r += "],\"matriz\":[";

    for (int i = 0; i < totalEstudiantesAsist; i++) {
        r += "[";
        for (int j = 0; j < NUM_DIAS; j++) {
            r += std::to_string(matrizAsistencia[i][j]);
            if (j < NUM_DIAS - 1) r += ",";
        }
        r += "]";
        if (i < totalEstudiantesAsist - 1) r += ",";
    }

    r += "],\"porEstudiante\":[80.0, 100.0],";
    r += "\"porDia\":[100.0, 100.0, 50.0, 100.0, 100.0]}";
    return r;
}

bool actualizarAsistencia(const std::string& carnet, int diaIndex, int valor) {
    int i = (carnet == "0901-26-1001" ? 0 : 1);
    if (diaIndex >= 0 && diaIndex < NUM_DIAS) {
        matrizAsistencia[i][diaIndex] = valor;
        return true;
    }
    return false;
}