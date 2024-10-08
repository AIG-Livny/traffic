class Point:
    def __init__(self, x, y) -> None:
        self.x = x
        self.y = y

class Direction:
    Forward = True
    Backward = False

class Lane:
    def __init__(self) -> None:
        self.direction : Direction  = Direction.Forward
        self.can_go_left            = False
        self.can_go_right           = True
        self.speed_limit            = 60

class Road:
    def __init__(self, points : list[Point]) -> None:
        self.points = points
        self.lanes : list[Lane] = [Lane(), Lane(), Lane(), Lane()]

road = Road([Point(0,0), Point(100,100), Point(300,200)])

import pygame

def drawRoad(road:Road):
    prev_pnt = None
    for pnt in road.points:
        if prev_pnt != None:
            pygame.draw.aaline(screen, square_color, (prev_pnt.x,prev_pnt.y), (pnt.x,pnt.y))
        prev_pnt = pnt

# Инициализация Pygame
pygame.init()

# Размеры окна
width = 600
height = 400

# Создание окна
screen = pygame.display.set_mode((width, height))

square_color = (255, 0, 0)  # Красный
square_size = 50
square_x = 100
square_y = 100
speed = 5

# Основной цикл игры
running = True
while running:
    # Обработка событий
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Обновление позиции квадрата
    square_x += speed

    # Проверка выхода за границы окна
    if square_x + square_size > width or square_x < 0:
        speed *= -1  # Изменение направления движения

    # Очистка экрана
    screen.fill((0, 0, 0))  # Черный фон

    pygame.draw.rect(screen, square_color, (square_x, square_y, square_size, square_size))
    drawRoad(road)

    # Обновление экрана
    pygame.display.flip()

    # Задержка для регулировки скорости
    pygame.time.delay(100)

# Выход из Pygame
pygame.quit()