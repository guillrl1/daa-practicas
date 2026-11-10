using System.Diagnostics;
using DAA.Base;
using DAA.Producto;

namespace DAA.Implementaciones;

public class ProductoPorFilas : Algoritmo {
    public override Solucion Resolver(Instancia instancia) {
        var datos = (DatosMatriz)instancia;
        int n = datos.Dimension;
        int[,] res = new int[n, n];

        var crono = Stopwatch.StartNew();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    res[i, j] += datos.MatrizA[i, k] * datos.MatrizB[k, j];
                }
            }
        }
        crono.Stop();

        return new ResultadoMatriz(res, crono.Elapsed.TotalMilliseconds);
    }
}