#include "qte.h"
#include "implicits.h"
#include "ui_interface.h"
#include "sdf.h"
#include "surface.h"
#include <iostream>
#include <chrono>
#include <stdlib.h>

using namespace std;

int RX, RY, RZ, compteurV;
double scale_factorX, scale_factorY, scale_factorZ, TX, TY, TZ;
double compteurDiff = 1;
double xDiff, yDiff, zDiff;
ID_Mesh id_m;
Cube * cubeDiff;
DifferenceSmooth * diff;
unsigned int nb_hits;

MainWindow::MainWindow() : QMainWindow(), uiw(new Ui::Assets)
{
    // Chargement de l'interface
    uiw->setupUi(this);

    // Chargement du GLWidget
    meshWidget = new MeshWidget;
    QGridLayout* GLlayout = new QGridLayout;
    GLlayout->addWidget(meshWidget, 0, 0);
    GLlayout->setContentsMargins(0, 0, 0, 0);
    uiw->widget_GL->setLayout(GLlayout);

    // Creation des connect
    CreateActions();

    meshWidget->SetCamera(Camera(Vector(10, 0, 0), Vector(0.0, 0.0, 0.0)));
}

MainWindow::~MainWindow()
{
    delete meshWidget;
}

void MainWindow::CreateActions()
{
    // Buttons
    connect(uiw->boxMesh, SIGNAL(clicked()), this, SLOT(BoxMeshExample()));
    connect(uiw->sphere, &QPushButton::clicked, [=]() {
        spawnSphere();
        id_m = ID_SPHERE;
    });
    connect(uiw->cube, &QPushButton::clicked, [=]() {
        spawnCube();
        id_m = ID_CUBE;
    });
    connect(uiw->cone, &QPushButton::clicked, [=]() {
        spawnCone();
        id_m = ID_CONE;
    });
    connect(uiw->tore, &QPushButton::clicked, [=]() {
        spawnTore();
        id_m = ID_TORE;
    });
    connect(uiw->capsule, &QPushButton::clicked, [=]() {
        spawnCapsule();
        id_m = ID_CAPSULE;
    });
    connect(uiw->union_, &QPushButton::clicked, [=]() {
        mixUnion();
    });
    connect(uiw->intersection, &QPushButton::clicked, [=]() {
        mixIntersection();
    });
    connect(uiw->difference, &QPushButton::clicked, [=]() {
        mixDifference();
    });
    connect(uiw->union_lisse, &QPushButton::clicked, [=]() {
        mixUnionSmooth();
    });
    connect(uiw->intersection_lisse, &QPushButton::clicked, [=]() {
        mixIntersectionSmooth();
    });
    connect(uiw->difference_lisse, &QPushButton::clicked, [=]() {
        mixDifferenceSmooth();
    });
    connect(uiw->composition, &QPushButton::clicked, [=]() {
        CompositionVisage();
        //id_m = ID_VISAGE;
    });
    connect(uiw->colonne, &QPushButton::clicked, [=]() {
        CompositionColonne();
    });
    connect(uiw->colonne_erosion, &QPushButton::clicked, [=]() {
        ColonneErosion();
    });
    connect(uiw->bezier_button, &QPushButton::clicked, [=]() {
        Bezier();
    });
    connect(uiw->bezier_button_2, &QPushButton::clicked, [=]() {
        Bezier_Twista();
    });
    connect(uiw->bezier_button_3, &QPushButton::clicked, [=]() {
        Bezier_Twistb();
    });

    connect(uiw->resetcameraButton, SIGNAL(clicked()), this, SLOT(ResetCamera()));
    connect(uiw->wireframe, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->radioShadingButton_1, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->radioShadingButton_2, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->RotateXslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->RotateYslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->RotateZslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->ScaleXBox, SIGNAL(valueChanged(double)), this, SLOT(meshScale()));
    connect(uiw->ScaleYBox, SIGNAL(valueChanged(double)), this, SLOT(meshScale()));
    connect(uiw->ScaleZBox, SIGNAL(valueChanged(double)), this, SLOT(meshScale()));
    connect(uiw->TranslateXBox, SIGNAL(valueChanged(double)), this, SLOT(meshTranslation()));
    connect(uiw->TranslateYBox, SIGNAL(valueChanged(double)), this, SLOT(meshTranslation()));
    connect(uiw->TranslateZBox, SIGNAL(valueChanged(double)), this, SLOT(meshTranslation()));

    // Widget edition
    connect(meshWidget, SIGNAL(_signalEditSceneLeft(const Ray&)), this, SLOT(editingSceneLeft(const Ray&)));
    connect(meshWidget, SIGNAL(_signalEditSceneRight(const Ray&)), this, SLOT(editingSceneRight(const Ray&)));
    connect(meshWidget, SIGNAL(_signalErosion(const Ray&)), this, SLOT(editingErosion(const Ray&)));
}

void MainWindow::editingSceneLeft(const Ray&)
{
}

void MainWindow::editingSceneRight(const Ray&)
{
}

void MainWindow::editingErosion(const Ray& ray)
{

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    double distance = 0.0;

    const unsigned int typeErosion = 1;

    if (sdf.Intersect(ray, distance))
    {
        Mesh implicitMesh;

        const Vector intersectionPoint = ray.Origin() + distance * ray.Direction();

        if (typeErosion == 1)
        {
            // �rosion incr�mentale
            unsigned int sphereSize = 0.6;
            //unsigned int sphereSize = 0.9;
            currentNode = new DifferenceSmooth(currentNode, new Sphere(intersectionPoint, sphereSize), 1);
            sdf = Signed(currentNode);
        }
        else if (typeErosion == 2)
        {
            // �rosion par paquets
            constexpr double smallSphereSize = 0.05;
            constexpr double circleRadius = 0.7;
            constexpr unsigned int numSmallSpheres = 5;

            currentNode = new DifferenceSmooth(currentNode, new Sphere(intersectionPoint, smallSphereSize), 1);

            for (int i = 0; i < numSmallSpheres; i++)
            {
                double angle = (2 * M_PI * i) / numSmallSpheres;
                double xOffset = circleRadius * cos(angle);
                double zOffset = circleRadius * sin(angle);

                currentNode = new DifferenceSmooth(currentNode, new Sphere(intersectionPoint + Vector(xOffset, 0, zOffset), smallSphereSize), 1);
            }

            sdf = Signed(currentNode);

        }

        sdf.Polygonize(50, implicitMesh, Box(5.0));

        vector<Color> cols;
        cols.resize(implicitMesh.Vertexes());
        for (size_t i = 0; i < cols.size(); i++)
            cols[i] = Color(0.8, 0.8, 0.8);

        meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
        UpdateGeometry();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Temps éxecution erosion manuelle = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

}

void MainWindow::editingErosion(const Ray& ray, float size_sphere, int typeErosion)
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    double distance = 0.0;

    if (sdf.Intersect(ray, distance))
    {
        //std::cout << "Le rayon a touché le sdf !" << std::endl;
        nb_hits++;

        Mesh implicitMesh;

        const Vector intersectionPoint = ray.Origin() + distance * ray.Direction();

        if (typeErosion == 1)
        {
            // �rosion incr�mentale
            unsigned int sphereSize = size_sphere;
            currentNode = new DifferenceSmooth(currentNode, new Sphere(intersectionPoint, sphereSize), 1);
            sdf = Signed(currentNode);
        }
        else if (typeErosion == 2)
        {
            // �rosion par paquets
            constexpr unsigned int numSmallSpheres = 5;
            double smallSphereSize = size_sphere / numSmallSpheres;
            constexpr double circleRadius = 0.8;


            currentNode = new DifferenceSmooth(currentNode, new Sphere(intersectionPoint, smallSphereSize), 1);

            for (int i = 0; i < numSmallSpheres; i++)
            {
                double angle = (2 * M_PI * i) / numSmallSpheres;
                double xOffset = circleRadius * cos(angle);
                double zOffset = circleRadius * sin(angle);

                currentNode = new DifferenceSmooth(currentNode, new Sphere(intersectionPoint + Vector(xOffset, 0, zOffset), smallSphereSize), 1);
            }

            sdf = Signed(currentNode);

        }

        sdf.Polygonize(50, implicitMesh, Box(5.0));

        vector<Color> cols;
        cols.resize(implicitMesh.Vertexes());
        for (size_t i = 0; i < cols.size(); i++)
            cols[i] = Color(0.8, 0.8, 0.8);

        meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
        UpdateGeometry();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    //std::cout << "Temps éxecution erosion manuelle = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

}

void MainWindow::BoxMeshExample()
{
    Mesh boxMesh = Mesh(Box(1.0));

    std::vector<Color> cols;
    cols.resize(boxMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(double(i) / 6.0, fmod(double(i) * 39.478378, 1.0), 0.0);

    meshColor = MeshColor(boxMesh, cols, boxMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCube()
{
    Mesh implicitMesh;
    currentNode = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
    sdf = Signed(currentNode);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnTore()
{
    Mesh implicitMesh;
    currentNode = new Tore(1, 3);
    sdf = Signed(currentNode);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnSphere()
{
    Mesh implicitMesh;
    currentNode = new Sphere(Vector(0, 0, 0), 2);
    sdf = Signed(currentNode);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCone()
{
    Mesh implicitMesh;
    currentNode = new Cone(0.2, 0.6, 5);
    sdf = Signed(currentNode);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCapsule()
{
    Mesh implicitMesh;
    currentNode = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
    sdf = Signed(currentNode);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::on_RotateXslider_valueChanged(int value)
{
    RX = value;
}

void MainWindow::on_RotateYslider_valueChanged(int value)
{
    RY = value;
}

void MainWindow::on_RotateZslider_valueChanged(int value)
{
    RZ = value;
}

void MainWindow::on_ScaleXBox_valueChanged(double arg1)
{
    scale_factorX = arg1;
}

void MainWindow::on_ScaleYBox_valueChanged(double arg1)
{
    scale_factorY = arg1;
}

void MainWindow::on_ScaleZBox_valueChanged(double arg1)
{
    scale_factorZ = arg1;
}

void MainWindow::on_TranslateXBox_valueChanged(double arg1)
{
    TX = arg1;
}

void MainWindow::on_TranslateYBox_valueChanged(double arg1)
{
    TY = arg1;
}

void MainWindow::on_TranslateZBox_valueChanged(double arg1)
{
    TZ = arg1;
}

void MainWindow::meshRotation()
{
    Mesh implicitMesh;
    Sphere * sphere_rotationX;
    Cube * cube_rotationX;
    Cone * cone_rotationX;
    Tore * tore_rotationX;
    Capsule * capsule_rotationX;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_rotationX = new Sphere(Vector(0, 0, 0), 2);
        currentNode = new Rotation(sphere_rotationX, Vector(RX, RY, RZ));
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube_rotationX = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        currentNode = new Rotation(cube_rotationX, Vector(RX, RY, RZ));
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone_rotationX = new Cone(0.2, 0.6, 5);
        currentNode = new Rotation(cone_rotationX, Vector(RX, RY, RZ));
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore_rotationX = new Tore(1, 3);
        currentNode = new Rotation(tore_rotationX, Vector(RX, RY, RZ));
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule_rotationX = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        currentNode = new Rotation(capsule_rotationX, Vector(RX, RY, RZ));
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::meshScale()
{
    Mesh implicitMesh;
    Sphere * sphere_scale;
    Cube * cube_scale;
    Cone * cone_scale;
    Tore * tore_scale;
    Capsule * capsule_scale;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_scale = new Sphere(Vector(0, 0, 0), 2);
        currentNode = new Scale(sphere_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube_scale = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        currentNode = new Scale(cube_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone_scale = new Cone(0.2, 0.6, 5);
        currentNode = new Scale(cone_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore_scale = new Tore(1, 3);
        currentNode = new Scale(tore_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule_scale = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        currentNode = new Scale(capsule_scale, Vector(scale_factorX, scale_factorY, scale_factorZ));
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::meshTranslation()
{
    Mesh implicitMesh;
    Sphere * sphere_translate;
    Cube * cube_translate;
    Cone * cone_translate;
    Tore * tore_translate;
    Capsule * capsule_translate;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_translate = new Sphere(Vector(0, 0, 0), 2);
        currentNode = new Translation(sphere_translate, Vector(TX, TY, TZ));
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube_translate = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        currentNode = new Translation(cube_translate, Vector(TX, TY, TZ));
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone_translate = new Cone(0.2, 0.6, 5);
        currentNode = new Translation(cone_translate, Vector(TX, TY, TZ));
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore_translate = new Tore(1, 3);
        currentNode = new Translation(tore_translate, Vector(TX, TY, TZ));
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule_translate = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        currentNode = new Translation(capsule_translate, Vector(TX, TY, TZ));
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixUnion()
{
    Mesh implicitMesh;
    Sphere * sphere_Union;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 1);
        currentNode = new Union(sphere, sphere_Union);
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Union(cube, sphere_Union);
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Union(cone, sphere_Union);
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Union(tore, sphere_Union);
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Union(capsule, sphere_Union);
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixUnionSmooth()
{
    Mesh implicitMesh;
    Sphere * sphere_Union;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new UnionSmooth(sphere, sphere_Union, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new UnionSmooth(cube, sphere_Union, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new UnionSmooth(cone, sphere_Union, 4);
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new UnionSmooth(tore, sphere_Union, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Union = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new UnionSmooth(capsule, sphere_Union, 4);
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixIntersection()
{
    Mesh implicitMesh;
    Sphere * sphere_Intersection;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Intersection(sphere, sphere_Intersection);
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Intersection(cube, sphere_Intersection);
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Intersection(cone, sphere_Intersection);
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Intersection(tore, sphere_Intersection);
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Intersection(capsule, sphere_Intersection);
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixIntersectionSmooth()
{
    Mesh implicitMesh;
    Sphere * sphere_Intersection;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new IntersectionSmooth(sphere, sphere_Intersection, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new IntersectionSmooth(cube, sphere_Intersection, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new IntersectionSmooth(cone, sphere_Intersection, 4);
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new IntersectionSmooth(tore, sphere_Intersection, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Intersection = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new IntersectionSmooth(capsule, sphere_Intersection, 4);
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixDifference()
{
    Mesh implicitMesh;
    Sphere * sphere_Difference;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Difference(sphere, sphere_Difference);
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Difference(cube, sphere_Difference);
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Difference(cone, sphere_Difference);
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Difference = new Sphere(Vector(1.5, 0, 0), 2);
        currentNode = new Difference(tore, sphere_Difference);
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new Difference(capsule, sphere_Difference);
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::mixDifferenceSmooth()
{
    Mesh implicitMesh;
    Sphere * sphere_Difference;
    Sphere * sphere;
    Cube * cube;
    Cone * cone;
    Tore * tore;
    Capsule * capsule;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere = new Sphere(Vector(-1, 0, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new DifferenceSmooth(sphere, sphere_Difference, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CUBE:
        cube = new Cube(Vector(-1, 0, 0), Vector(3, 3, 3));
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new DifferenceSmooth(cube, sphere_Difference, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CONE:
        cone = new Cone(0.2, 0.6, 4.5);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new DifferenceSmooth(cone, sphere_Difference, 4);
        sdf = Signed(currentNode);
        break;
    case ID_TORE:
        tore = new Tore(1, 3);
        sphere_Difference = new Sphere(Vector(1.5, 0, 0), 2);
        currentNode = new DifferenceSmooth(tore, sphere_Difference, 4);
        sdf = Signed(currentNode);
        break;
    case ID_CAPSULE:
        capsule = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sphere_Difference = new Sphere(Vector(1, 0, 0), 2);
        currentNode = new DifferenceSmooth(capsule, sphere_Difference, 4);
        sdf = Signed(currentNode);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::CompositionVisage()
{
    std::chrono::steady_clock::time_point begin_toilettes = std::chrono::steady_clock::now();
    Mesh implicitMesh;

    const double smoothFactor = 4;

    Tore * toreBouche = new Tore(1, 3);
    Sphere * sphereBouche = new Sphere(Vector(1, 0, 0), 2);
    IntersectionSmooth * interBouche = new IntersectionSmooth(toreBouche, sphereBouche, smoothFactor);
    Translation * translateBouche = new Translation(interBouche, Vector(0, 2, 0));

    Cone * coneNez = new Cone(0.2, 0.6, 4.5);
    Sphere * sphereNez = new Sphere(Vector(1, 0, 0), 2);
    Intersection * interNez = new Intersection(coneNez, sphereNez);
    Translation * translationNez = new Translation(interNez, Vector(3, 2, 0));

    Cone * cou = new Cone(0.2, 0.6, 4.5);
    Sphere * tete = new Sphere(Vector(1, 0, 0), 2);
    Sphere * eye1 = new Sphere(Vector(4, 1.5, 1.5), 2);
    Sphere * eye1b = new Sphere(Vector(15, 4.1, 4.1), 2);
    Scale * scaleEye1 = new Scale(eye1, Vector(0.5, 0.5, 0.5));
    Scale * scaleEye1b = new Scale(eye1b, Vector(0.2, 0.2, 0.2));
    Sphere * eye2 = new Sphere(Vector(4, 1.5, -1.5), 2);
    Sphere * eye2b = new Sphere(Vector(15, 4.1, -4.1), 2);
    Scale * scaleEye2 = new Scale(eye2, Vector(0.5, 0.5, 0.5));
    Scale * scaleEye2b = new Scale(eye2b, Vector(0.2, 0.2, 0.2));

    UnionSmooth * unionCouTete = new UnionSmooth(cou, tete, smoothFactor);
    UnionSmooth * unionVisageNez = new UnionSmooth(unionCouTete, translationNez, smoothFactor);
    UnionSmooth * unionVisageEye1 = new UnionSmooth(unionVisageNez, scaleEye1, smoothFactor);
    UnionSmooth * unionVisageEye2 = new UnionSmooth(unionVisageEye1, scaleEye2, smoothFactor);
    DifferenceSmooth * diffeye1visage = new DifferenceSmooth(unionVisageEye2, scaleEye1b, smoothFactor);
    DifferenceSmooth * diffeye2visage = new DifferenceSmooth(diffeye1visage, scaleEye2b, smoothFactor);
    DifferenceSmooth * visageComplet = new DifferenceSmooth(diffeye2visage, translateBouche, smoothFactor);

    Sphere * sphereTete = new Sphere(Vector(1, 3, 0), 1);
    UnionSmooth * tetenez = new UnionSmooth(visageComplet, sphereTete, smoothFactor);
    Sphere * spherelevre = new Sphere(Vector(1, 0, 0), 1.2);
    UnionSmooth * unionFinal = new UnionSmooth(tetenez, spherelevre, smoothFactor);

    currentNode = new Rotation(unionFinal, Vector(0, 90, 90));

    sdf = Signed(currentNode);
    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
    std::chrono::steady_clock::time_point end_toilettes = std::chrono::steady_clock::now();
    std::cout << "Temps éxecution CompositionToilettes() = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_toilettes - begin_toilettes).count() << "[ms]" << std::endl;

}

void MainWindow::CompositionColonne() {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    Mesh implicitMesh;

    const double smoothFactor = 1;
    int nb_reinures = 8;

    float radius_colonne = 1.5;
    float hauteur_colonne = 8.0;

    float step = (2 * M_PI) / nb_reinures;
    Cylinder * corps_colonne = new Cylinder(Vector(0.0, 0.0, 0.0), radius_colonne, hauteur_colonne);
    Cylinder * vide = new Cylinder(Vector(0.0, 0.0, 0.0), 0.0, 0.0);

    UnionSmooth * interReinures = new UnionSmooth(corps_colonne, vide, smoothFactor);
    for(int i = 0; i < nb_reinures; i++){
        float alpha = step * i;
        float x = radius_colonne * cos(alpha);
        float z = radius_colonne * sin(alpha);

        Cylinder * cylindreReinures = new Cylinder(Vector(x, 0, z), 0.4, hauteur_colonne);
        interReinures = new UnionSmooth(interReinures, cylindreReinures, smoothFactor);
    }
    float largeur_socle = (radius_colonne * 2) * 1.5;
    Cube * socle = new Cube(Vector(0, 0, 0), Vector(largeur_socle, hauteur_colonne / 8, largeur_socle));
    Cylinder * rebord_socle = new Cylinder(Vector(0.0, 0.0, 0.0), 0.2, largeur_socle);

    Rotation * rotation_rebord = new Rotation(rebord_socle, Vector(0.0, 0.0, 90.0));
    Translation * pos_rebord = new Translation(rotation_rebord, Vector(0.0, -hauteur_colonne / 16, -largeur_socle/2));
    UnionSmooth * colonne_it = new UnionSmooth(socle, pos_rebord, smoothFactor);
    rotation_rebord = new Rotation(pos_rebord, Vector(0.0, 0.0, 0.0));

    for(int j =0; j < 4; j++){
        rotation_rebord = new Rotation(rotation_rebord, Vector(0.0, 90.0, 0.0));
        colonne_it = new UnionSmooth(colonne_it, rotation_rebord, smoothFactor);
    }

    Translation * descendSocle = new Translation(colonne_it, Vector(0.0,-4.0, 0.0));
    UnionSmooth * final1 = new UnionSmooth(interReinures, descendSocle, smoothFactor);

    float largeur_sommet = (radius_colonne * 2) * 1.7;
    Cube * sommet = new Cube(Vector(0, 0, 0), Vector(largeur_socle, hauteur_colonne / 6, largeur_socle));
    Cylinder * rebord_sommet_union = new Cylinder(Vector(0.0, 0.0, 0.0), 0.2, largeur_socle);
    Cylinder * rebord_sommet_diff = new Cylinder(Vector(0.0, 0.0, 0.0), 0.2, largeur_socle);

    Rotation * rotation_sommet_union = new Rotation(rebord_sommet_union, Vector(0.0, 0.0, 90.0));
    Translation * pos_sommet_union = new Translation(rotation_sommet_union, Vector(0.0, -hauteur_colonne / 12, -largeur_socle/2));
    UnionSmooth * sommet_union = new UnionSmooth(sommet, pos_sommet_union, smoothFactor);

    Rotation * rotation_sommet_diff = new Rotation(rebord_sommet_diff, Vector(0.0, 0.0, 90.0));
    Translation * pos_sommet_diff = new Translation(rotation_sommet_diff, Vector(0.0, -hauteur_colonne / 12, -largeur_socle/2));
    DifferenceSmooth * sommet_diff = new DifferenceSmooth(sommet_union, pos_sommet_diff, smoothFactor);

    rotation_sommet_union = new Rotation(pos_sommet_union, Vector(0.0, 0.0, 0.0));
    rotation_sommet_diff = new Rotation(pos_sommet_diff, Vector(0.0, 0.0, 0.0));

    for(int j =0; j < 4; j++){
        rotation_sommet_union = new Rotation(rotation_sommet_union, Vector(0.0, 90.0, 0.0));
        sommet_union = new UnionSmooth(sommet_diff, rotation_sommet_union, smoothFactor);

        rotation_sommet_diff = new Rotation(rotation_sommet_diff, Vector(0.0, 90.0, 0.0));
        sommet_diff = new DifferenceSmooth(sommet_union, rotation_sommet_diff, smoothFactor);
    }

    Translation * monte_sommet = new Translation(sommet_diff, Vector(0.0, 4.3, 0.0));
    UnionSmooth * final = new UnionSmooth(final1, monte_sommet, smoothFactor);

    currentNode = new Rotation(final, Vector(0, 0, 0));

    sdf = Signed(currentNode);
    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();


    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Temps éxecution CompositionColonne() = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

}

void MainWindow::ColonneErosion(){
    std::chrono::steady_clock::time_point begin_erosion = std::chrono::steady_clock::now();
    // On construit en premier la colonne
    CompositionColonne();

    nb_hits = 0;
    // On crée un certain nombre de rayons qui vont éroder notre colonne
    // On part du principe qu'ils suivent une direction globale (et ne peuvent dévier que selon un angle (exemple érosion due aux vent/ marées)

    //int nb_spheres = 20;
    int nb_spheres = 20;


    // origine sera la même pour tous les rayons
    Vector origine1 = Vector(-12.0, 10.0, 2.0);
    Vector sommet_colonne = Vector(0.0,4.0*1.15, 0.0);
    Vector direction = Normalized(sommet_colonne - origine1);

    Ray ray = Ray(origine1, Normalized(direction));

    std::vector<Ray> rayons;
    std::vector<int> tailles_sphere;

    rayons.push_back(ray);
    tailles_sphere.push_back(2.0);

    for(int u =0; u < nb_spheres; u++){
        //srand((unsigned)time(NULL));
        float random_x = (rand() % 101 - 50) / 10.0;
        float random_y = (rand() % 101 - 50) / 10.0;
        float random_z = (rand() % 101 - 50) / 10.0;
        Vector origine = Vector(random_x, random_y, random_z);
        Vector new_dir = Normalized(Vector(Vector(0.0, 0.0, 0.0) - origine));
        Ray new_ray = Ray(origine, new_dir);
        rayons.push_back(new_ray);
        float proba = (rand() % 6 + 4) / 10.0;
        float sphere_size = 1.5 * proba;
        tailles_sphere.push_back(sphere_size);
    }

    for(int r = 0; r < rayons.size(); r++) {
        Ray rayon = rayons[r];
        std::cout << "Rayon actuel : origine(" << rayon.Origin() << ")   et   direction(" << rayon.Direction() << ")" << std::endl;
        int size = tailles_sphere[r];
        editingErosion(rayon, size, 1);
    }

    std::chrono::steady_clock::time_point end_erosion = std::chrono::steady_clock::now();
    std::cout << "Temps execution ColonneErosion() = " << std::chrono::duration_cast<std::chrono::milliseconds> (end_erosion - begin_erosion).count() << "[ms]" << std::endl;
    std::cout << " Sur " << nb_spheres << " lancés : " << nb_hits << " ont touché le sdf !" << std::endl;
}

void MainWindow::Bezier()
{
    std::vector<std::vector<Vector>> control_points;
    std::vector<Vector> row_1;
    row_1.push_back(Vector(-2.0, -2.0, 0.0));
    row_1.push_back(Vector(-2.0, 2.0, 0.0));
    std::vector<Vector> row_3;
    row_3.push_back(Vector(2.0, -2.0, 0.0));
    row_3.push_back(Vector(2.0, 2.0, 0.0));
    control_points.push_back(row_1);
    control_points.push_back(row_3);
    BezierSurface surface(control_points, 50, 50);
    surface.Twist(15.0);
    Mesh m = surface.Polygonize();

    std::vector<Color> cols;
    cols.resize(m.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(m, cols, m.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::Bezier_Twista()
{
    std::vector<std::vector<Vector>> control_points;
    std::vector<Vector> row_1;
    row_1.push_back(Vector(-2.0, -2.0, 0.0));
    row_1.push_back(Vector(-2.0, 2.0, 0.0));
    std::vector<Vector> row_3;
    row_3.push_back(Vector(2.0, -2.0, 0.0));
    row_3.push_back(Vector(2.0, 2.0, 0.0));
    control_points.push_back(row_1);
    control_points.push_back(row_3);
    BezierSurface surface(control_points, 50, 50);
    surface.Twist(6.0);
    Mesh m = surface.Polygonize();

    std::vector<Color> cols;
    cols.resize(m.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(m, cols, m.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::Bezier_Twistb()
{
    std::vector<std::vector<Vector>> control_points;
    std::vector<Vector> row_1;
    row_1.push_back(Vector(-2.0, -2.0, 0.0));
    row_1.push_back(Vector(-2.0, 2.0, 0.0));
    std::vector<Vector> row_3;
    row_3.push_back(Vector(2.0, -2.0, 0.0));
    row_3.push_back(Vector(2.0, 2.0, 0.0));
    control_points.push_back(row_1);
    control_points.push_back(row_3);

    BezierSurface surface(control_points, 50, 50);
    surface.Twist(2.0);
    Mesh m = surface.Polygonize();

    std::vector<Color> cols;
    cols.resize(m.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(m, cols, m.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::UpdateGeometry()
{
    meshWidget->ClearAll();
    meshWidget->AddMesh("BoxMesh", meshColor);

    uiw->lineEdit->setText(QString::number(meshColor.Vertexes()));
    uiw->lineEdit_2->setText(QString::number(meshColor.Triangles()));

    UpdateMaterial();
}

void MainWindow::UpdateMaterial()
{
    meshWidget->UseWireframeGlobal(uiw->wireframe->isChecked());

    if (uiw->radioShadingButton_1->isChecked())
        meshWidget->SetMaterialGlobal(MeshMaterial::Normal);
    else
        meshWidget->SetMaterialGlobal(MeshMaterial::Color);
}

void MainWindow::ResetCamera()
{
    meshWidget->SetCamera(Camera(Vector(-10.0), Vector(0.0)));
}
