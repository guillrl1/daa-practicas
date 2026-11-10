using DAA.Base;

namespace DAA.Producto;

public class ResultadoMatriz : Solucion {
    public int[,] MatrizRes { get; }
    private readonly double _milisegundos;

    public override double ValorObjetivo => _milisegundos;

    public ResultadoMatriz(int[,] res, double tiempo) {
        MatrizRes = res;
        _milisegundos = tiempo;
    }
}