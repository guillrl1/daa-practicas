using DAA.Base;

namespace DAA.Producto;

public class DatosMatriz : Instancia {
    public int[,] MatrizA { get; }
    public int[,] MatrizB { get; }
    public int Dimension { get; }

    public DatosMatriz(int[,] a, int[,] b, int n) {
        MatrizA = a;
        MatrizB = b;
        Dimension = n;
    }
}