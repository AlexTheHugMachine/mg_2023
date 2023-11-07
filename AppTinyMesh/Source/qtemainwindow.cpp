#include "qte.h"
#include "implicits.h"
#include "ui_interface.h"
#include "sdf.h"
#include "surface.h"
#include <iostream>

using namespace std;

int RX, RY, RZ, compteurV;
double scale_factorX, scale_factorY, scale_factorZ, TX, TY, TZ;
double compteurDiff = 1;
double xDiff, yDiff, zDiff;
ID_Mesh id_m;
Cube * cubeDiff;
DifferenceSmooth * diff;

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
    connect(uiw->bezier_button, &QPushButton::clicked, [=]() {
        Bezier();
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
}

void MainWindow::Bezier()
{
    std::vector<std::vector<Vector>> control_points;
    std::vector<Vector> row_1;
    row_1.push_back(Vector(-2.0, 0.0, -2.0));
    row_1.push_back(Vector(-2.0, 0.0, 0.0));
    row_1.push_back(Vector(-2.0, 0.0, 2.0));
    std::vector<Vector> row_2;
    row_2.push_back(Vector(0.0, 0.0, -2.0));
    row_2.push_back(Vector(0.0, 0.0, 0.0));
    row_2.push_back(Vector(0.0, 0.0, 2.0));
    std::vector<Vector> row_3;
    row_3.push_back(Vector(2.0, 0.0, -2.0));
    row_3.push_back(Vector(2.0, 0.0, 0.0));
    row_3.push_back(Vector(2.0, 0.0, 2.0));
    control_points.push_back(row_1);
    control_points.push_back(row_2);
    control_points.push_back(row_3);
    BezierSurface surface(control_points, 50, 50);
    surface.Twist(2.0);
    Mesh m = surface.BuildMesh();

    /*
    std::vector<Vector> control_points;
    control_points.push_back(Vector(0.0, 0.0, 0.1));
    control_points.push_back(Vector(0.2, 0.0, 0.7));
    control_points.push_back(Vector(0.4, 0.0, 0.1));
    control_points.push_back(Vector(0.6, 0.0, 0.7));
    control_points.push_back(Vector(1.0, 0.0, 0.3));


    RevolutionSurface surface(control_points, 50, 50);
    //surface.Twist(2.0);
    Mesh m = surface.BuildMesh();
    */

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
