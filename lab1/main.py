from graphics import *
from sympy.geometry import Point2D as syPoint
from sympy.geometry import Segment as sySegment
from sympy.geometry import Line as syLine


width = 400
height = 400


def generate_line_turned(point, intersection_point):
    global width
    return syLine(syPoint(point.x, point.y), syPoint(width, intersection_point.y + 1))


def does_intersect(line, line_to_intersect):
    intersection_list = line.intersect(line_to_intersect)
    return len(intersection_list) != 0


def equals_with_precision(point1, point2):
    return abs(point1.x - point2.x) < 1 and abs(point1.y - point2.y) < 1


def contains(polygonPoints, point):
    global width

    prev_point = syPoint(0, 0)
    check_line = sySegment(syPoint(point.x, point.y), syPoint(width, point.y))
    counter = 0

    for i in range(len(polygonPoints)):
        p = syPoint(polygonPoints[i].x, polygonPoints[i].y)

        if prev_point == syPoint(0, 0):
            prev_point = p
            continue

        side = sySegment(syPoint(prev_point.x, prev_point.y), p)
        intersects = does_intersect(side, check_line)

        if intersects:
            intersection_point = side.intersection(check_line)[0]
            if intersection_point != prev_point and intersection_point != p:
                counter += 1
            # elif intersection_point == prev_point and intersection_point == p:
            #     continue
            elif intersection_point == p:
                new_line = generate_line_turned(point, p)
                new_intersects1 = does_intersect(new_line, side)
                new_intersects2 = does_intersect(new_line,
                                                 sySegment(p,
                                                           syPoint(polygonPoints[(i + 1) % len(polygonPoints)].x,
                                                                   polygonPoints[(i + 1) % len(polygonPoints)].y)))

                if new_intersects1 and new_intersects2 or not new_intersects1 and not new_intersects2:
                    continue
                else:
                    counter += 1

        if i == len(polygonPoints) - 1:
            first_side = sySegment(p, syPoint(polygonPoints[0].x, polygonPoints[0].y))
            first_intersects = does_intersect(first_side, check_line)
            if first_intersects:
                intersection_point = first_side.intersection(check_line)[0]

                if intersection_point != prev_point and not equals_with_precision(intersection_point, polygonPoints[0]):
                    counter += 1
                # elif intersection_point == prev_point and equals_with_precision(intersection_point, polygonPoints[0]):
                #     continue
                elif equals_with_precision(intersection_point, polygonPoints[0]):
                    new_line = generate_line_turned(point, intersection_point)

                    new_intersects1 = does_intersect(new_line, first_side)
                    new_intersects2 = \
                        does_intersect(new_line,
                                       sySegment(syPoint(polygonPoints[0].x, polygonPoints[0].y),
                                                 syPoint(polygonPoints[1].x, polygonPoints[1].y)))
                    if new_intersects1 and new_intersects2 or not new_intersects1 and not new_intersects2:
                        continue
                    else:
                        counter += 1

        prev_point = p

    if counter % 2 == 1:
        return True
    else:
        return False


def draw_point_after_double_click(point):
    if draw_point_after_double_click.prevPoint is not None:
        draw_point_after_double_click.prevPoint.undraw()

    draw_point_after_double_click.prevPoint = draw_point(point)


draw_point_after_double_click.prevPoint = None


def draw_point(point):
    global win
    p = Point(point.x, point.y)
    p.draw(win)
    return p


def on_click(point):
    global win
    global eps
    global width
    global result_text

    if on_click.previousPoint == 0:
        on_click.previousPoint = point
    elif on_click.finished:
        draw_point_after_double_click(point)
        result_text.undraw()
        if contains(on_click.points, draw_point_after_double_click.prevPoint):
            result_text.setText("Inside polygon")
            result_text.draw(win)
        else:
            result_text.setText("Outside polygon")
            result_text.draw(win)
        return
    elif abs(point.x - on_click.previousPoint.x) < eps or abs(point.y - on_click.previousPoint.y) < eps:
        polygon = Polygon(on_click.points)
        polygon.draw(win)
        polygon.setFill('red')
        on_click.finished = True
        return

    p = draw_point(point)
    on_click.points.append(p)
    on_click.previousPoint = p


on_click.previousPoint = 0
on_click.points = []
on_click.finished = False


def main():
    global win
    global width
    global height
    global result_text

    win = GraphWin("Lab 1 : detecting if point belongs to polygon", width, height)
    text = Text(Point(width/2, 10), "Click for dot, double click to finish polygon")
    text.draw(win)
    win.setMouseHandler(on_click)
    win.getKey()
    win.close()


result_text = Text(Point(width / 2, 30), "")
win = None
eps = 2
main()
